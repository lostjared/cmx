#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#include<cstring>
#include<cassert>
#include "cmx_system.h"
#include "cmx_video.h"


namespace cmx {
	namespace system {

		int width,height;


		int init_system() {
			return 1;
		}

		int stop_system() {
			return 0;
		}

		unsigned int tick_system() {
			return 0;
		}

		void toggle_fullscreen() {
		}

		video::Surface *init_surface(int SCREEN_WIDTH, int SCREEN_HEIGHT) {
			return 0;
		}

		bool copy_surface(video::Surface &surface) {
			return true;
		}

		void *direct_system_memory() {

		}

		void swap_memory() {

		}

		void check_callbacks() {

		}
	}
}
