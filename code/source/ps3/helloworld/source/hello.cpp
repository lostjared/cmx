#include<cmx.h>
#include"fixedsys.bin.h"

int main(int argc, char **argv) {
	cmx::font::Font temp_font;
	if(cmx::system::init_system() != 1) return -1;
	cmx::input::init_joystick();
	if(!temp_font.loadFont(fixedsys_bin, sizeof(fixedsys_bin)))
		return -1;
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
		if(cmx::input::poll_joystick(0, cmx::input::BUTTON_TRIANGLE)) active = false;
		temp_font.printString( (unsigned int*) cmx::system::direct_system_memory(), cmx::system::width, 25, 25, 30, 25, _RGB(255,255,255), "Hello World!");
		cmx::system::swap_memory();
	}
	return cmx::system::stop_system();
}
