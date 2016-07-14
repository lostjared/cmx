#ifndef __CMXGL_H__
#define __CMXGL_H__

#include "cmx_video.h"
#include "cmx_system.h"

namespace cmx {

	namespace gl {
		cmx::video::Surface *init_gl_buffer(int w=1280, int h=720);
		void toggle_fullscreen();
		void swap_gl_buffers();
		void draw_surface(cmx::video::Surface *surface);
	}

}







#endif
