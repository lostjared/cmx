/* CrossMX - Debug Screen */
#include<cmx.h>
#include<cmx_debug.h>
#include<cassert>
#include"fixedsys.bin.h"
#define printf debugscr.printf


int main(int argc, char **argv) {
	if(cmx::system::init_system() != 1)
		return 0;
	cmx::input::init_joystick();
	cmx::video::Surface *surface=cmx::system::init_surface();
	assert(surface!=0);
	int off_x = 100, off_y = 100;
	cmx::debug::DebugScreen debugscr(off_x, off_y, surface->w, surface->h);
	cmx::font::Font font;
	assert(font.loadFont(fixedsys_bin, sizeof(fixedsys_bin)) == true);
	debugscr.initFont(&font, 45, 45);
	printf("Test of output\n 1 2 3\nPress Triangle to Exit, press X to display, right on d-pad for dummy text");
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
		for(int i = 0; i < cmx::input::num_joystick(); i++) {
			if(poll_joystick(i, cmx::input::BUTTON_TRIANGLE)) {
				active = false;
				printf("Goodbye");
				break;
			}

			if(poll_joystick(i, cmx::input::BUTTON_CROSS)) {
				printf("You pressed X!\n");
			}

			if(poll_joystick(i, cmx::input::BUTTON_RIGHT)) {
				printf("Test");
			}
		}
		unsigned int *buffer = (unsigned int*)cmx::system::direct_system_memory();
		memset(buffer, 0, surface->w*surface->h*4);
		debugscr.drawDebugScreen(_RGB(255, 255, 255), buffer, surface->w);
		cmx::system::swap_memory();
	}
	return cmx::system::stop_system();
}
