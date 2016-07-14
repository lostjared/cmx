#ifndef CMX_FONT_H__
#define CMX_FONT_H__

#define littleToBig(i) ((i&0xff)<<24)+((i&0xff00)<<8)+((i&0xff0000)>>8)+((i>>24)&0xff)
#define GetFX(m,x,nw) ((float)x * (float)m->mx / (float)nw);
#define GetFZ(m,y,nh) ((float)y * (float)m->my / (float)nh);
#define CMX_GetFW(old_w, x, new_w) (x*old_w/ new_w)
#define CMX_GetFH(old_h, y, new_h) (y*old_h/ new_h)

#include"cmx_video.h"

namespace cmx {

	namespace font {

		class Font {
		public:
			Font() { font_width = 0, font_height = 0; }
			~Font() {
				if(font_width==0) return;
				for(int chr=0; chr<=127; ++chr) {
					for(int z=0; z<font_width; ++z)
						free(font_source.letters[chr].fnt_ptr[z]);
					free(font_source.letters[chr].fnt_ptr);
				}
			}

			bool loadFont(const void *fnt_data, unsigned int size);
			bool loadFont(const char *src);

			struct _Font {
				int mx;
				int my;
				int tcolor;
				struct letter {
				    int **fnt_ptr;
				} letters[128];
			} font_source;

			int font_width, font_height;

		protected:
			void initFont(int width, int height, int color) {
					font_width = width, font_height = height;
					cmx::font::Font::_Font *m = &font_source;
					int l,p,i,z;
					 m->mx = width;
					 m->my = height;
					 for(l = 0; l <= 127; l++) {
						  m->letters[l].fnt_ptr = (int**) malloc ( sizeof(int*) * width);
						  for(p = 0; p < width; p++) {
							   m->letters[l].fnt_ptr[p] = (int*) malloc ( sizeof(int) * height);
						  }
						  for(i = 0; i < width; i++) {
							   for(z = 0; z < height; z++) {
									m->letters[l].fnt_ptr[i][z] = color;
							   }
						  }
					 }
					 m->tcolor = 0x0;
			}
			public:
			void printString(unsigned int *buffer, int buffer_width, int x, int y, int w, int h, unsigned int color,  const char *src) {
				_Font *fnt = &font_source;
			     int prev_x = x;
				 int offset_x = prev_x, offset_y = y;
				 int width = 0, height = 0;
				 int i,z,p;
				 for(p = 0; p < (int)strlen(src);  p++) {
					  if(src[p] == '\n') {
						   offset_x = prev_x;
						   offset_y += height;
						   continue;
					  }
					  width=0;

					  if(src[p] == ' ') {
						  offset_x += 7;
						  continue;
					  }

					  for(i = 0; i < fnt->mx; ++i) {
						   for(z = 0; z < fnt->my; ++z) {
							   int index = (int)src[p];
							   if(fnt->letters[index].fnt_ptr[i][z] != fnt->tcolor) {
								   buffer[(offset_x+i)+((offset_y+z)*buffer_width)] = color;
								   width=i;
								   if(height < z)
									   height=z;
								   }
							   }
						  }
						  offset_x += width + 2;
					}
				}
		};
	}
}

#endif

