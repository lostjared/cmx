#include<cmx.h>
#include"cmx_browser.h"
#include<cassert>
#include"font.mxf.h"

extern "C" int main(int argc, char **argv) {


	if(cmx::system::init_system() != 1) return 0;
	cmx::video::Surface *surface=cmx::system::init_surface();
	cmx::input::init_joystick();

	assert(surface != 0);
	bool active=true;
	cmx::font::Font font;
	font.loadFont((const void*)font_mxf, sizeof(font_mxf));
	cmx::debug::FileBrowser fb;
	std::string path;
#ifdef _WIN32
	path = "c:/";
#else
	path = "/";
#endif
	fb.setOptions(path, &font,surface->objectRect(), surface);
	while(active) {
		static cmx::event::Event e;
		while(e.procEvent()) {
			switch(e.type) {
			case cmx::event::EVENT_QUIT:
				active = false;
				break;
			case cmx::event::EVENT_KEYDOWN:
				break;
			default:
				break;
			}
		}
		surface->clearRect(surface->objectRect(), 0x0);
		fb.drawBrowser();
		static int btn[12][12];
		for(int i = 0; i < cmx::input::num_joystick(); ++i) {
			for(int z = 0; z < cmx::input::BUTTON_START; z++) {
				if(cmx::input::poll_joystick(i, z)) {
					if(btn[i][z] != 1) {
						btn[i][z] = 1;
						switch(z) {
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
				} else btn[i][z] = 0;
			}
		}
		cmx::system::copy_surface(*surface);
	}
	cmx::input::close_joystick();
	return cmx::system::stop_system();
}
