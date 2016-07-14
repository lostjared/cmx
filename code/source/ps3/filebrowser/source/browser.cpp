/*
 *
 * FileBrowser for PS3
 *
 */

#include<cmx.h>
#include<cmx_browser.h>
#include"fixedsys.bin.h"
#include<cassert>

void clrRect(unsigned int *buf, int w, int h, unsigned int col) {
	for(int i = 0; i < w; i++) {
		for(int z = 0; z < h; z++) {
			buf[i+(z*w)] = col;
		}
	}
}

int main(int argc, char **argv) {
	assert(cmx::system::init_system() == 1);
	cmx::video::Surface *surface=cmx::system::init_surface();
	cmx::input::init_joystick();
	cmx::font::Font the_font;
	the_font.loadFont((const void*)fixedsys_bin, sizeof(fixedsys_bin));
	bool active=true;
	cmx::debug::FileBrowser fb;
	fb.setPath("/");
	fb.setFont(&the_font);
	fb.setRect(surface->objectRect());
	while(active == true) {

		int pollButtons[] = { cmx::input::BUTTON_CROSS, cmx::input::BUTTON_CIRCLE, cmx::input::BUTTON_UP, cmx::input::BUTTON_DOWN, 0};
		static int buttons[5] = { 0 };

		for(int i = 0; i < 4; i++) {
			if(cmx::input::poll_joystick(0, pollButtons[i])) {
				if(buttons[i] != 1) {
					buttons[i] = 1;
					switch(pollButtons[i]) {
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
					}
				}
			} else buttons[i] = 0;
		}
		if(cmx::input::poll_joystick(0, cmx::input::BUTTON_TRIANGLE)) {
			active = false;
		}
		unsigned int *mem_buffer = (unsigned int*) cmx::system::direct_system_memory();
		clrRect(mem_buffer, cmx::system::width, cmx::system::height, 0x0);
		fb.setDrawBuffer(mem_buffer, cmx::system::width, cmx::system::height);
		fb.drawBrowser();
		cmx::system::swap_memory();
	}
	cmx::input::close_joystick();
	return cmx::system::stop_system();
}

