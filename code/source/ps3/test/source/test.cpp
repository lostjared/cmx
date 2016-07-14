#include<cmx.h>
#include"logo.bin.h"

using cmx::Rect;
using cmx::Point;
using cmx::Color;

int screenWidth=0;


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


int main(int argc, char **argv) {

	if(cmx::system::init_system() != 1) return 0;
	cmx::input::init_joystick();
	cmx::video::Surface *temp=0, image;
	temp = cmx::system::init_surface();
	screenWidth=temp->w;
	cmx::event::Event e;
	int rt = image.loadImage(logo_bin,sizeof(logo_bin));

	if(rt != 1 || image.buffer == 0) {
		return cmx::system::stop_system();
	}

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
		for(int i=0; i<cmx::input::num_joystick(); i++) {
			if(poll_joystick(i, cmx::input::BUTTON_CROSS)) {
				return cmx::system::stop_system();
			}
		}

		unsigned int *mem = (unsigned int*)cmx::system::direct_system_memory();
		copyImage(mem, image, 100, 100);
		cmx::system::swap_memory();
	}
	return cmx::system::stop_system();
}
