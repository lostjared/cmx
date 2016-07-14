/*

  (C) 2010 Jared Bruni under GPL

*/



#ifndef __CMX__H__SYSTEM
#define __CMX__H__SYSTEM

#include "cmx_video.h"


namespace cmx {
	namespace	system {
		extern int width, height;
		int init_system();
		int stop_system();
		unsigned int tick_system();
		video::Surface *init_surface();
		bool copy_surface(video::Surface &surface);
		void *direct_system_memory();
		void swap_memory();
	}

}



#endif

