/*
 * CrossMX demo application for PS3
 *
 */



#include<cmx.h>
#include<cstdio>
#include<cstdlib>
#include<ctime>
#include<sstream>
#include"logo.bin.h"
#include"block1.bin.h"
#include"block2.bin.h"
#include"block3.bin.h"
#include"block4.bin.h"
#include"block5.bin.h"
#include"block6.bin.h"
#include"block7.bin.h"
#include"block8.bin.h"
#include"block9.bin.h"
#include"block10.bin.h"
#include"fixedsys.bin.h"
#include "mpmut.h"

using cmx::Rect;
using cmx::Point;
using cmx::Color;

int screenWidth=0,screenHeight=0;
cmx::font::Font temp_font;
void copyImage(unsigned int *buffer, cmx::video::Surface&, int x, int y);
void clearRect(unsigned int *buf, Rect r, unsigned int color);
int offset_x=90;
int offset_y=50;

class gameScreen {
public:
	gameScreen() {}
	virtual void draw(unsigned int *buffer, unsigned int width) = 0;
	virtual void init() = 0;
};

class intro : public gameScreen {
public:
		virtual void init() {
			image.loadImage(logo_bin,sizeof(logo_bin));
		}
		virtual void draw(unsigned int *buffer, unsigned int width) {
			clearRect(buffer, Rect(0, 0, screenWidth, screenHeight), 0x0);
			copyImage(buffer, image, 448, 25);
			temp_font.printString((unsigned int*)buffer, screenWidth,460,820,40,40,0xFFFFFFFF, "Welcome to 'Mutatris' a Homebrew Demo using PSL1GHT for PS3");
			temp_font.printString((unsigned int*)buffer, screenWidth,460,870,40,40,0xFFFF0000, "Press Triangle at Any time to Exit - X to Play");
		}
protected:
		cmx::video::Surface image;

};

class game : public gameScreen {
public:

	cmx::video::Surface surfaces[12];
	virtual void init() {
		_game.newGame();
		_game.grid.clear();
		struct surfArr {
			const void *buf;
			unsigned int size;
		};
		surfArr game_blocks[] = {
				{block1_bin, sizeof(block1_bin) }, {block2_bin, sizeof(block2_bin)}, {block3_bin, sizeof(block3_bin)}, {block4_bin, sizeof(block4_bin)},
				{block5_bin, sizeof(block5_bin)}, {block6_bin, sizeof(block6_bin)}, {block7_bin, sizeof(block7_bin)}, {block8_bin, sizeof(block8_bin)},
				{block9_bin, sizeof(block9_bin)},{ block10_bin, sizeof(block10_bin) } };
		for(unsigned int i=0; i <9; i++) {
			surfaces[i].loadImage(game_blocks[i].buf, game_blocks[i].size);
		}

		srand((unsigned int)time(0));
	}

	void pollController() {
		static bool buttons[8][25];
		for(int i = 0; i < cmx::input::num_joystick(); ++i) {
			for(int z = 0; z < 8; ++z) {
				if(cmx::input::poll_joystick(i, z)) {
						if(buttons[i][z] == false) {
							buttons[i][z] = true;
							switch(z) {
							case cmx::input::BUTTON_UP:
								_game.shiftColor();
							break;
							}
					}
				} else buttons[i][z] = false;
			}

			if(cmx::input::poll_joystick(i, cmx::input::BUTTON_LEFT)) {
				_game.moveLeft();
			}
			if(cmx::input::poll_joystick(i, cmx::input::BUTTON_RIGHT))
				_game.moveRight();

			if(cmx::input::poll_joystick(i, cmx::input::BUTTON_DOWN))
				_game.moveDown();

		}


		static int q=0;

		if( (++q%4)==0 ) {
			_game.update_moveDown();
		}

		_game.update();
	}

	void drawBlock(mp::gameBlock &current, unsigned int *buffer) {
		int pos_x = current.x;
        int pos_y = current.y;
        for(unsigned int z = 0; z < 4; z++)
        {
             mp::blockType *t = (mp::blockType *)&current.blocks[z];
             Rect rc (((t->x+pos_x)*32)+offset_x , ((t->y+pos_y)*16)+offset_y, 32 , 16);
             copyImage(buffer, surfaces[ t->index_color ], rc.x, rc.y);
        }
	}

	virtual void draw(unsigned int *buffer, unsigned int width) {
		pollController();
		clearRect(buffer, Rect(0, 0, screenWidth, screenHeight), 0x0);
		static int offset_x = 90;
		static int offset_y = 50;
		int i=0,z=0;
		for(i = 0; i < _game.GRID_W; ++i) {
			for(z = 0; z < _game.GRID_H; ++z) {
				int num;
				if(i < _game.grid.grid_w && z < _game.grid.grid_h)
				num = _game.grid.data[i][z];
				else return;

				if(num != 0) {
					copyImage(buffer, surfaces[num], offset_x+(i*32), offset_y+(z*16));
				}
			}

		}
		drawBlock(_game.current, buffer);
		std::ostringstream stream;
		int middle=1080/2-100;
		stream << "Score: " << _game.nscore << " Cleared: " << _game.nclear;
		temp_font.printString(buffer,screenWidth, middle, 990, 45,45,0xFF0000FF,stream.str().c_str() );
	}
	mp::mxMut _game;
};

void copyImage(unsigned int *buf, cmx::video::Surface &surf, int x, int y) {
	int lines=0, z=0;
	unsigned int *buffer = (unsigned int*)surf.buffer;
	unsigned int *src_buffer = buf;
	src_buffer += x+(y*screenWidth);
	while(lines < surf.h) {
		for(z = 0; z < surf.w; ++z) {
			src_buffer[z] = buffer[z];
		}
		src_buffer += screenWidth;
		buffer += surf.pitch >> 2;
		++lines;
	}
}

void clearRect(unsigned int *buf, Rect r, unsigned int color) {
	for(int i = r.x; i < r.x+r.w; ++i)
		for(int z = r.y; z < r.y+r.h; z++)
			buf[i+(z*screenWidth)] = color;
}

game _game_;
intro _intro_;
gameScreen *scr;

int main(int argc, char **argv) {

	if(cmx::system::init_system() != 1) return 0;
	cmx::input::init_joystick();
	cmx::video::Surface *temp=0;
	temp = cmx::system::init_surface();
	screenWidth=temp->w;
	screenHeight=temp->h;
	bool ret = temp_font.loadFont(fixedsys_bin, sizeof(fixedsys_bin));
	if(ret == false) return cmx::system::stop_system();

	_intro_.init();
	_game_.init();
	scr = &_intro_;

	cmx::event::Event e;
	bool active=true;
	while(active) {
		while(e.procEvent()) {
			switch(e.type) {
			case cmx::event::EVENT_QUIT:
				active = 0;
				break;
			default:
				break;
			}
		}

		if(_game_._game.is_gameOver()) {
			scr = &_intro_;
			_game_._game.newGame();
			_game_._game.grid.clear();
		}

		for(int i=0; i<cmx::input::num_joystick(); i++) {
			if(poll_joystick(i, cmx::input::BUTTON_TRIANGLE)) {
				return cmx::system::stop_system();
			}
			if(poll_joystick(i, cmx::input::BUTTON_CROSS)) {
				scr = &_game_;
				break;
			}
		}
		unsigned int *buffer = (unsigned int *) cmx::system::direct_system_memory();
		scr->draw(buffer, screenWidth);
		cmx::system::swap_memory();
	}
	return cmx::system::stop_system();
}
