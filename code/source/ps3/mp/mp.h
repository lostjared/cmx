#ifndef __MP__H__
#define __MP__H__

#define TILE_W 17
#define TILE_H 25
#include<cstring>

struct mpBlock {

	unsigned int block_a[3];
	int x,y;

};


class mpGame {

public:
	mpGame();
	void newGame();
	void moveLeft();
	void moveRight();
	void moveDown();
	void addScore();
	void shiftBlock();
	void nextBlock();
	void update();

	const int getScore() const;
	const int getLines() const;
	const bool isOver() const;

	int operator()(int x, int y);
	const mpBlock *getBlock() const;


protected:
	int Tiles[TILE_W+5][TILE_H+5];
	mpBlock gameblock;
	int score, lines;
	bool over;


	void proc_blocks();
	void merge_block();
	void proc_down();
	int bounds(int x, int y);
	void randBlock(mpBlock &b);

};


#endif

