/*
 *
 *
 * AlphaFLame HD (2D SDL implementation)
 * this program allows you to create images interactively.
 * consult the readme for more information.
 *
 * (C) 2011 Jared Bruni -> Licensed under the GPL
 *
 */

#include<cmx.h>
#include "afhd.h"
#include<ctime>
#include<cstdlib>
#include<cstring>
#include"fixedsys.bin.h"
#include<unistd.h>

using cmx::Rect;
using cmx::Point;



extern "C" int main(int argc, char **argv) {
	cmx::font::Font the_font;
	srand( (unsigned int) time(0) );
	int opt, rez = 720;
	bool full = false, slow = false;
	const char *file_str = 0, *prefix_str = 0, *path_str = 0;
	while ((opt = getopt(argc, argv, "fsi:p:e:m")) != -1) {
		  switch(opt) {
			   case 's':
				   slow = true;
			   break;
			   case 'f':
				   full = true;
			   break;
			   case 'i':
				   file_str = optarg;
				   break;
			   case 'p':
				   prefix_str = optarg;
				   break;
			   case 'e':
				   path_str = optarg;
				   break;
			   case 'm':
				   rez = 1080;
				   break;
		  }
	 }

	if(file_str == 0) {
		std::cout << argv[0] << ": usage\n-i source file\n-f fullscreen\n-s slow mode\n-p prefix snapshot prefix\n\n";
		exit(0);
	}

	if(argc < 2) {
		std::cout << argv[0] << ": usage\n-i source file\n-f fullscreen\n-s slow mode\n-p prefix snapshot prefix\n\n";
		exit(0);
	}

	if( cmx::system::init_system() != 1 ) return -1;
	cmx::video::Surface *surface;
	if(rez == 720) surface = cmx::system::init_surface();
	else surface = cmx::system::init_surface(1920,1080);
	cmx::video::Surface *red_blue = cmx::video::newSurface(surface->w, surface->h), *redcolor, *bluecolor;
	cmx::input::init_joystick();
	af af_x;
	af_x.buildImages(file_str, cmx::system::width, cmx::system::height);

	if(path_str != 0)
		chdir(path_str);

	if(full == true) {
		std::cout << "Running Fullscreen..\n";
		cmx::system::toggle_fullscreen();
	}

	if(slow == true) {
		std::cout << "Running in Slow Mode\n";
	}

	the_font.loadFont((const void*)fixedsys_bin, sizeof(fixedsys_bin));

	cmx::video::Surface *back_surface=cmx::video::newSurface(cmx::system::width, cmx::system::height);
	redcolor = cmx::video::newSurface(back_surface->w, back_surface->h);
	bluecolor = cmx::video::newSurface(back_surface->w, back_surface->h);
	af_x.buildStartImage(back_surface);
	bool active=true;
	bool key_up = true;
	bool snap = false;
	int af_blend2 = 0;
	bool red_blue_b=false;

	while(active == true) {

		static cmx::event::Event e;
		while(e.procEvent()) {
			switch(e.type) {
			case cmx::event::EVENT_QUIT:
				active = false;
				break;
			case cmx::event::EVENT_KEYDOWN:
				if(e.key == 27) active=false;
				if(e.key == 'a' && key_up == true) {
					snap = true;
					key_up = false;
				}

				switch(e.skey) {
				case cmx::event::K_DOWN:
					af_x.decSpeed();
					break;
				case cmx::event::K_UP:
					af_x.incSpeed();
					break;
				case cmx::event::K_LEFT:
					break;
				case cmx::event::K_RIGHT:
					break;
				}

				switch(e.key) {
				case '3':
					red_blue_b = true;
					break;
				case '0':
					red_blue_b = false;
					break;
				case 'r':
					af_x.fade = af::FADE_R;
					break;
				case 'g':
					af_x.fade = af::FADE_B;
					break;
				case 'b':
					af_x.fade = af::FADE_G;
					break;
				case 's':
					af_x.fade = af::FADE_RAND;
					break;
				case 'x':
					af_x.resetAlpha();
					break;
				case 'z':
					af_blend2 = 1;
					break;
				case 'o':
					af_blend2 = 0;
					break;
				case 't':
					af_blend2 = 2;
					break;
				case 'm':
					af_blend2 = 3;
					break;
				case 'u':
					af_blend2 = 4;
					break;
				case 'w':
					af_blend2 = 5;
					break;
				case 'q':
					af_blend2 = 6;
					break;
				case 'l':
					af_blend2 = 7;
					break;

				case 'i':
					af_x.Morph(back_surface);

					switch(af_blend2) {
					case 0:
						af_x.blendImage(back_surface, af_x.image_files[rand()%4].the_surface);
						break;
					case 1:
						af_x.blendImage2(back_surface);
						break;
					case 2:
						af_x.TriMorph(back_surface);
						break;
					case 3:
						af_x.ScrambleMe(back_surface);
						break;
					case 4:
						af_x.blendImage2(back_surface);
						af_x.TriMorph(back_surface);
						af_x.ScrambleMe(back_surface);
						break;
					case 5:
						af_x.TriangleShift(back_surface, back_surface);
						break;
					case 6:
						af_x.SquareBlend(back_surface);
						break;
					case 7:
						af_x.Lame(back_surface);
						break;
					}

				}

				break;
			case cmx::event::EVENT_KEYUP:

				if(e.key == 'a') key_up = true;


				break
				;
			default:
				break;
			}
		}
		unsigned int *buffer = (unsigned int *) surface->buffer;
		if(slow == false ) {
			af_x.Morph(back_surface);
			switch(af_blend2) {
			case 0:
				af_x.blendImage(back_surface, af_x.image_files[rand()%4].the_surface);
				break;
			case 1:
				af_x.blendImage2(back_surface);
				break;
			case 2:
				af_x.TriMorph(back_surface);
				break;
			case 3:
				af_x.ScrambleMe(back_surface);
				break;
			case 4:
				af_x.blendImage2(back_surface);
				af_x.TriMorph(back_surface);
				af_x.ScrambleMe(back_surface);
				break;
			case 5:
				af_x.TriangleShift(back_surface, back_surface);
				break;
			case 6:
				af_x.SquareBlend(back_surface);
				break;
			case 7:
				af_x.Lame(back_surface);
				break;
			}
		}
		surface->copySurface(*back_surface, back_surface->objectRect(), Rect(Point(0,0), back_surface->objectSize()));
		static float alpha = 0.1f;
		if(red_blue_b == false)
		cmx::system::copy_surface(*surface);
		else {
			static int cnt = 0;

			switch(cnt) {
			case 0:
				af_x.TransTo(surface, red_blue);
				cmx::system::copy_surface(*red_blue);
				break;
			case 1:
				af_x.TransToColor(surface, redcolor, 0);
				af_x.TransToColor(surface, bluecolor, 1);
				af_x.AlphaBlend(bluecolor, redcolor, alpha);
				cmx::system::copy_surface(*bluecolor);
				break;
			}
			cnt = !cnt;
			alpha += 0.001f;
		}
		if(snap == true) {
			the_font.printString((unsigned int*)surface->buffer, surface->w, 25, 25, 30, 25,0xFFFFFFFF, "LostSideDead.com - AlphaFlameHD");
			snap = false;
			std::ostringstream stream;
			static unsigned int i=0;
			++i;

			std::string addstr = "";
			if(prefix_str != 0) addstr = std::string(prefix_str) + "_";
			stream << addstr <<  "afhdss" << i << ".png";
			cmx::system::copy_surface(*surface);
			surface->saveImage(stream.str());
			std::cout << "Saved image: " << stream.str()<<"\n";
		}
	}
	delete red_blue;
	delete redcolor;
	delete bluecolor;
	delete back_surface;
	cmx::input::close_joystick();
	return cmx::system::stop_system();
}
