#include"cmx_types.h"

namespace cmx {
	Color::Color(unsigned char r, unsigned char g, unsigned char b) {
		color.color[0] = 0;
		color.color[1] = r;
		color.color[2] = g;
		color.color[3] = b;
	}
	Color CMX_RGB(unsigned char r, unsigned char g, unsigned char b) {
		Color col;
		col.color.color[0] = 0;
		col.color.color[1] = r;
		col.color.color[2] = g;
		col.color.color[3] = b;
		return col;
	}
}
