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
			void *buffer;
			int w,h,bpp,pitch;

			Surface() {
				buffer = NULL;
				w = 0, h = 0, bpp = 0;
				release_on = 1;
			}
			Surface(const Surface &c) {
				this->buffer = c.buffer;
				this->w = c.w;
				this->h = c.h;
				this->bpp = c.bpp;
				this->release_on = c.release_on;
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
				if(dst_rect.y+dst_rect.h > h)
					return;
				if(dst_rect.w+dst_rect.x > w)
					return;
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
	}
}

#endif

