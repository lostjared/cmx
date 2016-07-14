#ifndef __BROWSER_H__
#define __BROWSER_H__
#include"cmx.h"
#include<string>
#include<iostream>
#include<sstream>
#include<vector>
#include<cstring>
#include<cstdlib>
#include<unistd.h>
#include<sys/stat.h>
#include<unistd.h>
#include<cstdio>
#include<fcntl.h>


namespace cmx {
	namespace debug {
		class FileBrowser {
			std::string dir_path;
			cmx::font::Font *dir_font;
			cmx::Rect dir_rect;
			cmx::video::Surface *dir_surface;
			std::vector<std::string> array;
			int select_var, total_y, dir_offset;

			unsigned int *draw_buffer;
			int draw_width, draw_height;

		public:
			FileBrowser() {
				dir_path=".";
				dir_offset = 0;
				select_var = 0;
				total_y = 0;
			}
			FileBrowser(std::string path) {
				dir_path = path;
				dir_offset = 0;
				select_var = 0;
				total_y = 0;
			}

			void updateAtPath() {
				if(array.size()>0)
				array.erase(array.begin(), array.end());
				dir_offset = 0;
				select_var = 0;
				fillVector(array);
			}

			std::string atPath() { return dir_path; }

			void setPath(std::string path) { dir_path = path; updateAtPath(); }
			void setOptions(std::string path, cmx::font::Font *font, cmx::Rect rc, cmx::video::Surface *draw_surface) {
				setPath(path);
				setFont(font);
				setRect(rc);
				setDrawSurface(draw_surface);
			}
			void setFont(cmx::font::Font *df) {
				dir_font = df;
			}
			void setRect(cmx::Rect rc) {
				dir_rect = rc;
			}

			void setDrawBuffer(unsigned int *buf, int draw_width, int draw_height) {
				draw_buffer = buf;
				this->draw_width = draw_width;
				this->draw_height = draw_height;
			}

			void setDrawSurface(cmx::video::Surface *draw_surface) {
				dir_surface = draw_surface;
				draw_buffer = (unsigned int *)draw_surface->buffer;
				draw_width = draw_surface->w;
				draw_height = draw_surface->h;
			}
			void movementDown() {
				if(select_var+dir_offset < (int)array.size()-1) {
					if(total_y != 0 && select_var > total_y-2) {
						dir_offset++;
						return;
					}
					select_var++;
				}
			}
			void movementUp() {
				if(select_var > 0) {
					if(dir_offset > 0)
					{
						dir_offset--;
						return;
					}
					select_var--;
				}
			}
			void movementInward() {
				std::string selectedf = selectedFile();
				if(isDir(selectedf))
				setPath(selectedf);

			}
			void movementOutward() {
				if( (int)dir_path.rfind("/") != -1) {
					dir_path = dir_path.substr(0, dir_path.rfind("/"));
					setPath(dir_path);
				}
			}
			std::string selectedFile() {
				std::string str = array[select_var+dir_offset];
				std::ostringstream stream;
				stream << dir_path << "/" << str;
				return stream.str();
			}
			void drawBrowser() {
				int draw_x = 80, draw_y = 80;
				int index = 0;
				unsigned int color=0;
				while(index+dir_offset < (int)array.size()) {

					if (array[dir_offset+index] == "mxmap")
						color = _RGB(0,255,0);
					else if(select_var+dir_offset == index+dir_offset) color = _RGB(255,255,255);
					else color = _RGB(255, 0, 0);

					dir_font->printString(draw_buffer, draw_width,  draw_x, draw_y, 35, 30, color, array[dir_offset+index].c_str());
					index++;
					draw_y += 32;
					if(draw_y > (draw_height-80)) {
						total_y = index;
						break;
					} else total_y = 0;
				}
			}

			bool isDir(std::string fname);
			bool dirContainsMap() {
				for(int i = 0; i < (int)array.size(); ++i) {
					if(array[i] == "mxmap") return true;
				}
				return false;
			}
		protected:
			void fillVector(std::vector<std::string> &v);
		};
	}
}





#endif


