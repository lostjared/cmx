/*
 * MXLevelEdit Map Test -
 * this program will load a map (exported as text) from MXLevelEdit
 * graphics need to be png
 * A Work In Progress
 *
 *
 * -
 *
 *
 */


#include<cmx.h>
#include<cmx_parser.h>
#include<cmx_debug.h>
#include<cmx_browser.h>
#include<cassert>
#include<fstream>
#include<iostream>
#include "fixedsys.bin.h"

using cmx::Rect;

cmx::debug::DebugScreen dbgscr;

lvl::Level *initLevelParser(std::string path, std::string name, parse::MXLevelParser &parser, lexer::Scanner &scan) {
	lvl::Level *level=0;
	std::string txt;
	std::fstream file;
	file.open(name.c_str(), std::ios::in | std::ios::binary);
	if(file.is_open()) {
		char *buf=0;
		unsigned int len=0;
		file.seekg(0, std::ios::end);
		len = file.tellg();
		file.seekg(0, std::ios::beg);
		buf = new char [len+1];
		file.read((char*)buf, len);
		buf[len] = 0;
		txt = buf;
		delete [] buf;
		file.close();
		scan.setText(txt);
		parser.set(&scan);
		parser.collectTokens();
		level = parser.parse();
	} else {
		dbgscr.printf("Error could not load map from directory %s\n", name.c_str());
		return 0;
	}
	if(level == 0) {
		dbgscr.printf("Error could not load level\n");
		return 0;
	}
	if(level->loadLevelGraphics(path) == 0) {
		dbgscr.printf("Error graphics problems\n");
		return 0;
	}
	return level;
}

int main(int argc, char **argv) {
	if(cmx::system::init_system() != 1) return 0;
	cmx::video::Surface *surface=cmx::system::init_surface();
	cmx::input::init_joystick();
	assert(surface != 0);
	cmx::debug::FileBrowser fb;
	dbgscr.initRect(surface->objectRect());
	cmx::font::Font font;
	font.loadFont((const void*)fixedsys_bin, sizeof(fixedsys_bin));
	dbgscr.initFont(&font, 30, 25);
	fb.setPath("/");
	fb.setRect(surface->objectRect());
	fb.setFont(&font);

	enum {SCR_OP=1, SCR_MAP};
	int scr = SCR_OP;
	std::string txt="";
	std::string name="";
	parse::MXLevelParser parser;
	lexer::Scanner scan;
	lvl::Level *level=0;

	bool active=true;
	while(active == true) {
		static cmx::event::Event e;
		while(e.procEvent()) {
			switch(e.type) {
			case cmx::event::EVENT_QUIT:
				active = false;
				break;
			default:
				break;
			}
		}
		if(scr == SCR_MAP) {


			if(cmx::input::poll_joystick(0, cmx::input::BUTTON_UP)) {
				level->scroll(level->UP);
			}

			if(cmx::input::poll_joystick(0, cmx::input::BUTTON_DOWN)) {
				level->scroll(level->DOWN);
			}

			if(cmx::input::poll_joystick(0, cmx::input::BUTTON_LEFT)) {
				level->scroll(level->LEFT);
			}

			if(cmx::input::poll_joystick(0, cmx::input::BUTTON_RIGHT)) {
				level->scroll(level->RIGHT);
			}
			unsigned int *mem_buf = (unsigned int *)cmx::system::direct_system_memory();
			cmx::video::clrRect(mem_buf, cmx::system::width, cmx::system::height, 0x0);
			if(level != 0) level->drawLevel(mem_buf, cmx::system::width, cmx::system::height);
			dbgscr.drawDebugScreen(_RGB(255,255,255),(unsigned int*)mem_buf, cmx::system::width);
		} else if(scr == SCR_OP) {

			static int pollButtons[] = { cmx::input::BUTTON_UP, cmx::input::BUTTON_DOWN, cmx::input::BUTTON_CROSS, cmx::input::BUTTON_CIRCLE, cmx::input::BUTTON_START, 0 };
			static int buttons[14] = { 0 };


				for(int z = 0; pollButtons[z] != 0; z++) {
					if(cmx::input::poll_joystick(0, pollButtons[z])) {
						if(buttons[z] != 1) {
							buttons[z] = 1;
							switch(pollButtons[z]) {
							case cmx::input::BUTTON_UP:
								fb.movementUp();
								break;
							case cmx::input::BUTTON_DOWN:
								fb.movementDown();
								break;
							case cmx::input::BUTTON_CROSS:
								fb.movementInward();
								break;
							case cmx::input::BUTTON_CIRCLE:
								fb.movementOutward();
								break;
							case cmx::input::BUTTON_START:

									if(!fb.dirContainsMap())
										break;

									std::string path = fb.atPath();
									std::cout << path << "\n";
									if(fb.isDir(path)) {
										level = initLevelParser(path, path+"/mxmap/tiles.txt", parser, scan);
										if(level != 0) {
											dbgscr.printf("Successfully loaded level: %s\n", level->name.c_str());
										}
										else {
											dbgscr.printf("Could not load level at path: %s\n", path.c_str());
										}
									}
									scr = SCR_MAP;

								break;
							}
					}
				} else buttons[z] = 0;
			}

			unsigned int *mem_buf = (unsigned int *) cmx::system::direct_system_memory();
			cmx::video::clrRect(mem_buf, cmx::system::width, cmx::system::height, 0x0);
			fb.setDrawBuffer(mem_buf, cmx::system::width, cmx::system::height);
			fb.drawBrowser();
			if(fb.dirContainsMap()) {
				font.printString(mem_buf, cmx::system::width, 250, 45, 25, 20, _RGB(0,255,0), "Directory Contains Map - Press Start");
			}
		}
		cmx::system::swap_memory();
	}
	cmx::input::close_joystick();
	return cmx::system::stop_system();
}
