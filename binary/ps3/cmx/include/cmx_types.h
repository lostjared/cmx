/*

  (C) 2010 Jared Bruni under GPL

*/



#ifndef __CMX__H__TYPES
#define __CMX__H__TYPES
#include<string>
#include<iostream>
#include<sstream>
#include<cstdlib>
#include "cmx_error.h"

#define _RGB(r,g, b) cmx::CMX_RGB(r, g, b).color.color_value

namespace cmx {
	class Color {
	public:
		union color_type {
			unsigned char color[4];
			unsigned int color_value;
		} color;
		Color() { color.color_value = 0; }
		Color(const Color &c) { color = c.color; }
		Color(unsigned int color_value) { color.color_value = color_value; }
		Color(unsigned char r, unsigned char g, unsigned char b);
		Color(color_type &col) {
			color = col;
		}
		void setValue(Color &from_color) {
			color = from_color.color;
		}
	};

	Color CMX_RGB(unsigned char r, unsigned char g, unsigned char b);

	class Point {
	public:
		int x,y;
		Point() { x = y = 0; }
		Point(int x,int y) { this->x = x; this->y = y; }
		Point(const Point &p) {  x = p.x, y = p.y; }
		Point &operator=(const Point &p) {
			x = p.x;
			y = p.y;
			return *this;
		}
	};

	class Rect : public Point {
	public:
		int w, h;
		Rect() {
			x = y = w = h = 0;
		}
		Rect(Point origin, Point size) {
			x = origin.x;
			y = origin.y;
			w = size.x;
			h = size.y;
		}
		Rect(int x, int y, int w, int h) { this->x = x, this->y = y, this->w = w, this->h = h; }
		Rect &operator=(const Rect &r) {
			x = r.x;
			y = r.y;
			w = r.w;
			h = r.h;
			return *this;
		}
	};

}


#endif

