#include"cmx_types.h"
#include"cmx_video.h"

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
			r = c.color.color[0];
			g = c.color.color[1];
			b = c.color.color[2];
		}
}


