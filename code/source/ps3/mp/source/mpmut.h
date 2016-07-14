#ifndef __MX_MP_MUT_
#define __MX_MP_MUT_
#include<cstdlib>
#include<cstring>

namespace mp
{

	class blockType {
	public:

		blockType() { x = 0, y = 0; index_color = 0;}
		blockType(int x, int y, unsigned int index_color) { this->x = x, this->y = y, this->index_color = index_color; }
		blockType(const blockType &t);

		void operator=(const blockType &t);
		bool operator==(const blockType &t);

		int x,y;
		unsigned int index_color;
	};


	class gameBlock {

	public:


		enum block_t { BLOCK_VERT, BLOCK_HORIZ, BLOCK_SQUARE };

		int x,y;
		blockType blocks[4];
		block_t block_type;
	
		gameBlock() { x = 0; y = 0; }
		gameBlock(const gameBlock &c);
		void operator=(const gameBlock &b);
		void randBlock(); 	
	};

	class mxMut;


	template<size_t width, size_t height>
	class mpGrid {

	public:
		enum { DATA_WIDTH=width,DATA_HEIGHT=height };
		char data[width][height];
		int grid_w,grid_h;
		mpGrid(mxMut *m) { this->mut = m; grid_w =width; grid_h=height; memset(data, 0, sizeof(data)); }
		void addScore(int, int);
		void clear();
		void merge(const gameBlock &block);
		void update();
		void merge_down();
		int score, blocks_cleared;
		mxMut *mut;

	};


	class mxMut  {

	public:


		mxMut();

		void newGame();
		void moveLeft();
		void moveRight();
		void moveDown();
		void shiftColor();
		void nextBlock();

		const bool is_gameOver() const { return game_over; }

		int nscore, nclear;
		enum { GRID_W = 1790/32,
		       GRID_H = 900/16 };

		mpGrid<GRID_W, GRID_H> grid;
		gameBlock current, next;
		bool game_over;
		void addn(int ns, int nc) {  nscore = ns, nclear  = nc;  }
		const void freeze(bool t) { freez = t; }
		void update();

		bool freez;

		void update_moveDown();
	protected:
		void update_mergeBlock();

	};




	template<size_t width, size_t height>
	void mpGrid<width,height>::addScore(int score, int cleared) 
	{ 	
		this->score += score; 
		this->blocks_cleared += cleared; 
		mut->addn(score,cleared);
			
	}
		


	template<size_t width, size_t height>
	void mpGrid<width,height>::clear()
	{

		memset(data, 0, sizeof(data));


	}

	template<size_t width, size_t height>
	void mpGrid<width,height>::merge(const gameBlock &block)
	{

		int offset_x = block.x;
		int offset_y = block.y;
		for(unsigned int i = 0; i < 4; i++)
		{
			int cx = block.blocks[i].x;
			int cy = block.blocks[i].y;
			cx+=offset_x;
			cy+=offset_y;
			if(cx >= 0 && cx < static_cast<int>(width) && cy >= 0 && cy < static_cast<int>(height))
			{
				data[cx][cy] = block.blocks[i].index_color;
			}

		}

	}
	

	template<size_t width, size_t height>
	void mpGrid<width,height>::merge_down()
	{
		size_t i = 0, z = 0;


		for(i=0; i<width;i++)
		{
			for(z = 0; z < height-1; z++)
			{

				if(data[i][z] != 0 && data[i][z+1] == 0)
				{

					data[i][z+1] = data[i][z];
					data[i][z] = 0;
					continue;

				}

			}

		}

	}


	template<size_t width, size_t height>
	void mpGrid<width,height>::update()
	{

		size_t i,z;

		// left to right 4 in  a row vertical 
	
		for(i = 0; i < width-3; i++)
		{

			for(z = 0; z < height; z++)
			{
				int cur_item = data[i][z];
				if(cur_item == 0) continue;

				if(cur_item == data[i+1][z] && cur_item == data[i+2][z] && cur_item == data[i+3][z])
				{

					data[i][z] = 0;
					data[i+1][z] = 0;
					data[i+2][z] = 0;
					data[i+3][z] = 0;
					addScore(2,4);
					continue;
				}
			}

		}

		// horizontal lines

		for(i = 0; i < width; i++)
		{

			for(z = 0; z < height-3; z++)
			{
				int cur_item = data[i][z];
				if(cur_item == 0) continue;

				if(cur_item == data[i][z+1] && cur_item == data[i][z+2] && cur_item == data[i][z+3])
				{

					data[i][z] = 0;
					data[i][z+1] = 0;
					data[i][z+2] = 0;
					data[i][z+3] = 0;
					addScore(3,4);
				}
			}
		}



		//  removes squares

		for(i = 0; i < width-1; i++)
		{
			for(z = 0; z < height-1; z++)
			{
				int cur_item = data[i][z];
				if(cur_item == 0) continue;

				if(cur_item == data[i][z+1] && cur_item == data[i+1][z] && cur_item == data[i+1][z+1])
				{
					data[i][z] = 0;
					data[i+1][z] = 0;
					data[i+1][z+1] = 0;
					data[i][z+1] = 0;
					addScore(10,4);
				}
			}
		}

		merge_down();

	}

	

}


#endif




