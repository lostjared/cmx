#include"cmx_system.h"
#include "cmx_video.h"
#include "SDL.h"
#include<cassert>
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#include<cstring>
// 720

#define BYTES_PER_PIXEL 4

SDL_Surface *dst_surf;
SDL_Surface *copy_surf;

namespace cmx {
	namespace system {

		int width,height;
		Uint32 flags;


		SDL_Surface *CreateBuffer(size_t w, size_t h) {
			static int rmask, gmask, bmask, amask;
			amask = 0xFF000000;
		   	bmask = 0x00FF0000;
		   	gmask = 0x0000FF00;
		   	rmask = 0x000000FF;
			return  SDL_CreateRGBSurface(SDL_SWSURFACE , w, h, 32, rmask,gmask,bmask,amask);
		}

		int init_system() {
			if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER) < 0) {
				return 0;
			}
			return 1;
		}

		int stop_system() {
			if(copy_surf != 0) SDL_FreeSurface(copy_surf);
			copy_surf = 0;
			SDL_Quit();
			return 0;
		}

		unsigned int tick_system() {
			return SDL_GetTicks();
		}

		void toggle_fullscreen() {
			static bool full=false;
			full = !full;
			Uint32 flags=0;
			if(full) flags = SDL_FULLSCREEN;
			else flags = 0;
			dst_surf = SDL_SetVideoMode(cmx::system::width, cmx::system::height, 32, flags);
		}

		video::Surface *init_surface(int SCREEN_WIDTH, int SCREEN_HEIGHT) {
			video::Surface *surface = 0;
			try {
				surface = new video::Surface();
				surface->createSurface(SCREEN_WIDTH, SCREEN_HEIGHT, BYTES_PER_PIXEL);
				dst_surf = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0);
				SDL_WM_SetCaption("[cmx]", 0);
				copy_surf = CreateBuffer(dst_surf->w, dst_surf->h);
				width = SCREEN_WIDTH;
				height = SCREEN_HEIGHT;
				video::s = (void*) copy_surf;
				if(!dst_surf) throw Exception("Error could not create Generic Surface");
				return surface;
			} catch (Exception &e) {
				delete surface;
				std::cerr << e.errorString() << "\n";
				throw;
			}
			return 0;
		}

		bool copy_surface(video::Surface &surface) {
			if( SDL_MUSTLOCK(copy_surf) ) {
				SDL_LockSurface(copy_surf);
			}
			assert(surface.buffer != 0);
			void *buf = copy_surf->pixels;
			copy_surf->pixels = (void*)surface.buffer;
			if( SDL_MUSTLOCK(copy_surf)) {
				SDL_UnlockSurface(copy_surf);
			}
			SDL_FillRect(dst_surf, 0, 0x0);
			SDL_BlitSurface(copy_surf, 0, dst_surf, 0);
			SDL_UpdateRect(dst_surf, 0, 0, cmx::system::width, cmx::system::height);
			copy_surf->pixels = buf;
			return true;
		}

		void *direct_system_memory() {
			if(SDL_MUSTLOCK(copy_surf))
				SDL_LockSurface(copy_surf);

			return (void*)copy_surf->pixels;
		}

		void swap_memory() {
			if(SDL_MUSTLOCK(copy_surf))
			 	SDL_UnlockSurface(copy_surf);
			SDL_FillRect(dst_surf, 0, 0);
			SDL_BlitSurface(copy_surf, 0, dst_surf, 0);
			SDL_UpdateRect(dst_surf, 0, 0, dst_surf->w, dst_surf->h);
		}

		void check_callbacks() { }
	}
}
