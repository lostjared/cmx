#ifndef CMX_DEBUG_HEADER_H
#define CMX_DEBUG_HEADER_H

#include "cmx_video.h"
#include "cmx_font.h"
#include "cmx_types.h"

#include<string>
#include<cstdarg>
#include<cstdio>

namespace cmx {

	namespace debug {

		class DebugScreen {
			int string_offset;
		protected:
					int off_x, off_y, width, height, font_width, font_height;
					cmx::font::Font *m_fnt;
					std::string text;

		public:
			DebugScreen() : string_offset(0), off_x(0), off_y(0), width(0), height(0), font_width(0), font_height(0),  m_fnt(0), text("") {

			}

			DebugScreen(int offset_x, int offset_y, int w, int h) : string_offset(0), off_x(offset_x), off_y(offset_y), width(w), height(h), m_fnt(0) {
				m_fnt = 0;
			}

			void initRect(cmx::Rect r) {
				off_x = r.x;
				off_y = r.y;
				width = r.w;
				height = r.h;
			}

			void initFont(cmx::font::Font *fnt, int fnt_w, int fnt_h) {
				m_fnt = fnt;
				font_width = fnt_w;
				font_height = fnt_h;
			}

			void clear() {
				text = "";
			}

			void shiftLines() {
				if(string_offset == 0) return;
				text = text.substr(string_offset, text.length()-string_offset);

			}

			int drawDebugScreen(unsigned int color, unsigned int *buffer,unsigned int buffer_width) {

				int x = 50, y = 50;
				int w = font_width, h = font_height;

				cmx::font::Font::_Font *fnt = &m_fnt->font_source;
				const char *src = text.c_str();
				int prev_x = x;
				int offset_x = prev_x, offset_y = y;
				int width = 0, height = 0;
				int i, z, p;
				for (p = 0; p < (int) strlen(src); p++) {
					if (src[p] == '\n') {
						offset_x = prev_x;
						offset_y += height;
						continue;
					}
					if(offset_x > this->width-x) {
						offset_x = prev_x;
						offset_y += height;
						continue;
					}

					if(offset_y > this->height-height-y)
					{
						text = "";
						return 1;

					}
					for (i = 0; i < w; i++) {
						for (z = 0; z < h; z++) {
							int fX = GetFX(fnt, i, w);
							int fY = GetFZ(fnt, z, h);
							int index = (int) src[p];
							if (fnt->letters[index].fnt_ptr[fX][fY] != fnt->tcolor) {
								buffer[(offset_x + i) + ((offset_y + z) * buffer_width)] = color;
								width = i;
								if (height < z)
									height = z;
							}
						}
					}

					offset_x += width + 2;
				}

				return 0;
			}

			void printf(const char *src, ...) {
					va_list list;
					char buffer[8000];
					va_start(list, src);
					vsprintf((char*)buffer, src, list);
					va_end(list);
					text += buffer;
			}
		};
	}
}



#endif

