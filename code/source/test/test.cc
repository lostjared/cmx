#include"cmx.h"
#include"cmx_types.h"
#include<cassert>
#include<memory>

using cmx::Point;
using cmx::Rect;
using cmx::Color;

int main(int argc, char **argv) {
	cmx::video::Surface *temp_image=NULL;
	try {
		cmx::system::init_system();
		cmx::video::Surface *surface=cmx::system::init_surface();
		temp_image = new cmx::video::Surface();
		int rt = temp_image->loadImage("logo.png");
		assert (rt > 0 && temp_image->buffer != 0 && surface->buffer != 0);
		cmx::event::Event e;
		bool active = true;
		while(active == true) {
			while(e.procEvent()) {
				switch(e.type) {
				case cmx::event::EVENT_QUIT:
					active = false;
					break;
				case cmx::event::EVENT_KEYDOWN:
					//active = false;
					break;
				}
			}
			unsigned int *buffer = (unsigned int*)surface->buffer;
			for(int i = 0; i < surface->w; ++i) {
				for(int z = 0; z < surface->h; ++z) {
					buffer[i+z*surface->w] = _RGB(rand()%255, rand()%255, rand()%255);
				}
			}
			surface->copySurface(*temp_image, temp_image->objectRect(), Rect(Point(120,120), Point(temp_image->objectSize())));
			cmx::system::copy_surface(*surface);
		}
		if(temp_image) delete temp_image;
		return cmx::system::stop_system();
	} catch (cmx::Exception &e) {
		if(temp_image) delete temp_image;
		std::cerr << e.errorString() << "\n";
	} catch (std::exception &e) {
		if(temp_image) delete temp_image;
		std::cerr << e.what() << "\n";
	} catch (...) {
		std::cerr << "unknown error aborting\n";
	}

	// if (surface != 0) delete surface;
	return cmx::system::stop_system();
}
