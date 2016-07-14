#ifndef MXLVL_E_H_
#define MXLVL_E_H_
#include<string>
#include<sstream>
#include<fstream>
#include<cmx.h>

namespace lvl {

	struct Item {
		std::string value;
		std::string fname;
		int cx,cy,cw,ch;
		cmx::video::Surface *surface;
		Item(std::string value, std::string cfname, int x, int y, int w, int h) {
			cx = x;
			cy = y;
			cw = w;
			ch = h;
			this->value = value;
			fname = cfname;
		}
		Item() : value(""), fname(""), cx(0), cy(0), cw(0), ch(0) {}
	};

	class Level {
	public:
		std::string name;
		int level_width, level_height, level_max_x, level_max_y;
		Level() : items(0),offset_x(0), offset_y(0), block_width(16), block_height(16) { }
		~Level() {
			if(items == 0) return;
			for(int z = 0; z < level_max_y; z++) {
				delete [] items[z];
			}
			delete [] items;
		}

		struct temp {
			std::string fname;
			std::string value;
			int offset;
			temp(std::string f, std::string v, int o) {
				fname = f;
				value = v;
				offset = o;
			}

			temp() : fname(""), value(""), offset(0) {}
		};

		std::map<std::string, temp> files;

		int loadLevelGraphics(std::string path) {
			if(items == 0) {
				std::cerr << "Error failure..\n";
				return 0;

			}
			unsigned int i = 0;
			for(i = 0; i < sprites.size(); i++) {
				files[sprites[i].fname].fname = sprites[i].fname;
				files[sprites[i].fname].value = sprites[i].value;
				files[sprites[i].fname].offset = i;
			}
			std::map<std::string, temp>::iterator it;
			for(it = files.begin(); it != files.end(); ++it) {
				std::ostringstream stream;
				stream << path << "/mxmap/img/" << it->first;
				int rt = surfaces[it->first].loadImage(stream.str());
				if(rt != 1) {
					std::cerr << "Error loading graphic: " << stream.str() << "\nAborting ...\n";
					return 0;
				} else std::cout << "Graphic: " << stream.str() << " has been loaded..\n";
			}
			{
				for(int i = 0; i < level_max_x; i++) {
					for(int z =0; z < level_max_y; z++) {
						if(items[i][z] != 0) {
								items[i][z]->surface = &surfaces[items[i][z]->fname];
						}
					}
				}
			}
			std::cout << "successfully loaded level: " << name << "\n";
			return 1;
		}

		int loadLevelGraphics(std::map<std::string, void *> &data) {
			if(items == 0) {
				std::cerr << "Error failure..\n";
				return 0;

			}
			unsigned int i = 0;
			for(i = 0; i < sprites.size(); i++) {
				files[sprites[i].fname].fname = sprites[i].fname;
				files[sprites[i].fname].value = sprites[i].value;
				files[sprites[i].fname].offset = i;
			}
			std::map<std::string, temp>::iterator it;
			for(it = files.begin(); it != files.end(); ++it) {
				std::ostringstream stream;
				stream << "from resource: " << it->first << "\n";
				int rt = surfaces[it->first].loadImage(data[it->first], sizeof(data[it->first]));;
				if(rt != 1) {
					std::cerr << "Error loading graphic: " << stream.str() << "\nAborting ...\n";
					return 0;
				} else std::cout << "Graphic: " << stream.str() << " has been loaded..\n";
			}
			{
				for(int i = 0; i < level_max_x; i++) {
					for(int z =0; z < level_max_y; z++) {
						if(items[i][z] != 0) {
								items[i][z]->surface = &surfaces[items[i][z]->fname];
						}
					}
				}
			}
			std::cout << "successfully loaded level: " << name << "\n";
			return 1;
		}

		void drawLevel(cmx::video::Surface *surface) {
			static int plot_x = (surface->w-level_width)/2, plot_y=(surface->h-level_height)/2;
			static int i = 0, z = 0;
			for(i = 0; i < (int)level_width/(int)block_width; i++) {
				for(z = 0; z < int(level_height/block_height); z++) {
					if(i+offset_x < level_max_x && z+offset_y < level_max_y && items[i+offset_x][z+offset_y] != 0) {
						lvl::Item *item=items[i+offset_x][z+offset_y];
						cmx::video::Surface *surf=items[i+offset_x][z+offset_y]->surface;
						surface->copySurface(*surf, cmx::Rect(item->cx,item->cy, item->cw, item->ch),cmx::Rect((offset_x+i*block_width)+plot_x, (offset_y+z*block_height)+plot_y, block_width, block_height));
					}
				}
			}
		}

		enum DIRECTION { UP=0, DOWN=1, LEFT=2, RIGHT=3 };

		void scroll(DIRECTION dir) {
			switch(dir) {
			case UP:
				if(offset_y > 0) --offset_y;
				break;
			case DOWN:
				if(offset_y < level_max_y-(level_height/block_height)) ++offset_y;
				break;
			case LEFT:
				if(offset_x > 0) --offset_x;
				break;
			case RIGHT:
				if(offset_x < level_max_x-(level_height/block_width)) ++offset_x;
				break;
			}
		}
		std::vector<Item> sprites;
		std::map<std::string, cmx::video::Surface> surfaces;
		Item ***items;
		int offset_x, offset_y, block_width, block_height;
	};
}

#endif

