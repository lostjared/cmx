#include<cmx.h>
#include<cassert>
#include"font.mxf.h"


int main(int argc, char **argv) {
	assert( cmx::system::init_system() == 1 );
	cmx::video::Surface *front = cmx::system::init_surface();
	cmx::input::init_joystick(); // init joy after surface
	cmx::font::Font the_font;
	the_font.loadFont((const void*)font_mxf, sizeof(font_mxf));
	bool active = true;
	while(active) {
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

		if(cmx::input::poll_joystick(0, cmx::input::BUTTON_CROSS)) {
			active = false;
		}

		the_font.printString((unsigned int *)front->buffer, front->w, 35, 35, 30, 35, _RGB(255,255,255), "Hello World");
		cmx::system::copy_surface(*front);
	}
	cmx::input::close_joystick();
	return cmx::system::stop_system();
}
