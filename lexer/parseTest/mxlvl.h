#ifndef MXLVL_E_H_
#define MXLVL_E_H_
#include<string>
#include<sstream>
#include<fstream>

namespace lvl {

	struct Item {
		std::string value;
		std::string fname;
		int cx,cy,cw,ch;
		// cmx::video::Surface *surface;
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
		Level() : items(0) { }
		~Level() {
			if(items == 0) return;
			for(int z = 0; z < level_max_y; z++) {
				delete [] items[z];
			}
			delete [] items;
		}
		int loadLevel(std::string s);
		int loadLevelGraphics();
		std::vector<Item> sprites;
		Item ***items;
	};
}

#endif

