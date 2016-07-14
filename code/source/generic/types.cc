#include"cmx_types.h"
#include"cmx_video.h"
#include "SDL.h"
namespace cmx {

		Color::Color(unsigned char r, unsigned char g, unsigned char b) {
			color.color[0] = r;
			color.color[1] = g;
			color.color[2] = b;
			color.color[3] = 255;
		}

		Color CMX_RGB(unsigned char r, unsigned char g, unsigned char b) {
			Color col;
			col.color.color[0] = r;
			col.color.color[1] = g;
			col.color.color[2] = b;
			col.color.color[3] = 255;
		 	return col;
		}

		void CMX_COLOR(Color &c, unsigned char &r, unsigned char &g, unsigned char &b) {
			SDL_Color col;
			SDL_PixelFormat *f = ((SDL_Surface*)cmx::video::s)->format;
			SDL_GetRGB(c.color.color_value,f,&r, &g, &b);
		}
}


