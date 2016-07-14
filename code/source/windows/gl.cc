#include "cmx_gl.h"
#include "cmx_system.h"

namespace cmx {
	namespace gl {
	cmx::video::Surface *init_gl_buffer(int w, int h) {
			return cmx::system::init_surface(w,h);
		}

		void toggle_fullscreen() {
			cmx::system::toggle_fullscreen();
		}


		void draw_surface(cmx::video::Surface *surface) {
			cmx::system::copy_surface(*surface);
		}
	}

}
