/*

  (C) 2010 Jared Bruni under GPL

*/



#ifndef __CMX__H__VIDEO
#define __CMX__H__VIDEO

#include"cmx_types.h"
#include<memory>
#include<string>
#include<cstdlib>
#include<cstring>

namespace cmx {

	namespace video {

	extern void *s;

	class Surface {
		int release_on;
		public:
			int loadImage(std::string image);
			int loadImage(const void *buffer, unsigned int bsize);
			void saveImage(std::string image);

			void *buffer;
			int w,h,bpp,pitch;

			Surface() {
				buffer = NULL;
				w = 0, h = 0, bpp = 0;
				release_on = 1;
				pitch = 0;
			}
			Surface(const Surface &c) {
				this->buffer = c.buffer;
				this->w = c.w;
				this->h = c.h;
				this->bpp = c.bpp;
				this->release_on = c.release_on;
				this->pitch = c.pitch;
			}

			~Surface() {
				if(release_on)
				release();
			}

			void release() {
				if(buffer != NULL) {
					free(buffer);
					buffer = NULL;
				}
			}

			bool createSurface(int w, int h, int  bpp) {
				void *tmp = calloc(1, (w*h)*bpp);

				if(tmp == 0)
					throw Exception("Could not allocate Memory!");

				this->w = w, this->h = h, this->bpp = bpp;
				buffer = tmp;
				return true;
			}

			void copySurface(Surface &src, Rect src_rect, Rect dst_rect) {
				unsigned int *src_ptr = (unsigned int*) src.buffer;
				unsigned int *where = (unsigned int *) buffer;
				int lines=0;
				/*temporary until later: */
				if(dst_rect.y+dst_rect.h > h) {
					std::cerr << "Error \n";
					return;
				}
				if(dst_rect.w+dst_rect.x > w) {
					std::cerr << "Error \n";
					return;
				}
				while(lines <  dst_rect.h) {
					for(int i = 0; i < dst_rect.w; i++) {
						where[(i+dst_rect.x)+((dst_rect.y+lines)*w)] = src_ptr[i+lines*src.w];
					}
					 lines++;
				}
			}

			int clearRect(Rect surf_rect, int clear_color) {
				if(surf_rect.x+surf_rect.w > w) return -1;
				if(surf_rect.y+surf_rect.h > h) return -2;
				int lines = 0;
				while(lines < surf_rect.h) {
					memset(&((unsigned int*)buffer)[surf_rect.x+lines*surf_rect.w], clear_color,surf_rect.w*4);
					lines++;
				}
				return 1;
			}
			Rect objectRect() { return Rect(0, 0, w, h); }
			Point objectSize() { return Point(w,h); }

		protected:

		};

		Surface *newSurface(int w, int h);

		inline void copySurfaceToBuffer(unsigned int *buf, int width, int height, cmx::video::Surface &surf, Rect src_rect, Rect dst_rect, bool hasAlpha = false) {
			unsigned int *src_ptr = (unsigned int*) surf.buffer;
			unsigned int *where = (unsigned int *) buf;
			register int lines=0, i = 0;
			unsigned int color_key = 0x0, key_color = 0x0;
			if(hasAlpha == true) color_key = 0xFFFFFFFF;
			/*temporary until later: */
			if(dst_rect.y+dst_rect.h > height)
				return;
			if(dst_rect.w+dst_rect.x > width)
				return;

			if(hasAlpha == true) {

				while(lines <  dst_rect.h) {
					for(i = 0; i < dst_rect.w; ++i) {
						if((key_color = src_ptr[i+lines*surf.w]) != color_key)
							where[(i+dst_rect.x)+((dst_rect.y+lines)*width)] = key_color;
					}
					lines++;
				}
			} else {
				while(lines <  dst_rect.h) {
					for(i = 0; i < dst_rect.w; ++i) {
							where[(i+dst_rect.x)+((dst_rect.y+lines)*width)] = src_ptr[i+lines*surf.w];
						}
					lines++;
				}
			}
		}
		inline void clrRect(unsigned int *buf, int w, int h, unsigned int col) {
			register int x=0,y=0;
			for(y = 0; y < h; ++y) {
				for(x = 0; x < w; ++x) {
					buf[x+(y*w)] = col;
				}
			}
		}

		inline void copyImage(unsigned int *buf, cmx::video::Surface &surf, int screenWidth, int x, int y, int blockWidth, int blockHeight) {
		     int lines=0, z=0;
		     unsigned int *buffer = (unsigned int*)surf.buffer;
		     unsigned int *src_buffer = buf;
		     src_buffer += x+(y*screenWidth);
		     while(lines < blockHeight && lines < surf.h) {
		          for(z = 0; (z < blockWidth && z < surf.w); ++z) {
		               src_buffer[z] = buffer[z];
		          }
		          src_buffer += screenWidth;
		          buffer += surf.pitch >> 2;
		          ++lines;
		     }
		}

	}
}

#endif

