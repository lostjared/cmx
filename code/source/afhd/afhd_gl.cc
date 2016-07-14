/*
 *
 *
 *
 * AlphaFlame HD (OpenGL implementation)
 * this program creates images interactively
 * for more information consult the readme
 *
 * (C) 2011 Jared Bruni Licensed under GPL
 *
 *
 *   http://lostsidedead.com
 *
 */


#include<cmx.h>
#include<cmx_gl.h>
#if __APPLE__
#include<OpenGL/gl.h>
#include<OpenGL/glu.h>
#else
#include<GL/gl.h>
#include<GL/glu.h>
#endif

#include "afhd.h"
#include<ctime>
#include<cstdlib>
#include<cstring>
#include"fixedsys.bin.h"
#include<unistd.h>

using cmx::Rect;
using cmx::Point;
int times_pos = 2;
bool morph_on = true, morph_index=0;

void copy_surface(cmx::video::Surface &surf) {
	cmx::gl::draw_surface(&surf);
	cmx::gl::swap_gl_buffers();
}

typedef void (af::*func_ptr)(cmx::video::Surface *);
static func_ptr members[] = { &af::blendImage2, &af::TriMorph, 
	&af::ScrambleMe, &af::SquareBlend, &af::RGBMix, &af::G,
	&af::TriBlendParadox, &af::ThoughtThis
};


int af_blend2 = 0;
af af_x;

void checkInput(cmx::video::Surface *);

int main(int argc, char **argv) {
	cmx::font::Font the_font;
	srand( (unsigned int) time(0) );
	int opt;
	int mode = 720;
	bool full = false, slow = false;
	int image_mode = 1;
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
				   mode = 1080;
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
	if(mode == 720)
		surface = cmx::gl::init_gl_buffer();
	else
		surface = cmx::gl::init_gl_buffer(1920, 1080);

	const char *src = (const char*) glGetString(GL_VERSION);
	std::cout << "OpenGL Version: " << src << std::endl;
	cmx::video::Surface *red_blue = cmx::video::newSurface(surface->w, surface->h), *redcolor, *bluecolor;
	cmx::input::init_joystick();
	af_x.buildImages(file_str,cmx::system::width, cmx::system::height);

	if(path_str != 0)
		chdir(path_str);

	if(full == true) {
		std::cout << "Running Fullscreen..\n";
		cmx::gl::toggle_fullscreen();
	}
	glViewport(0, 0, surface->w, surface->h);
	glClearDepth(1.0f);
	glClearColor(0, 0, 0, 0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, surface->w, 0, surface->h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(slow == true)
		std::cout << "Running in Slow Mode\n";

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
	bool shift_it = false;
	bool rand_it = false;

	while(active == true) {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		static cmx::event::Event e;
		while(e.procEvent()) {
			switch(e.type) {
			case cmx::event::EVENT_QUIT:
				active = false;
				break;
			case cmx::event::EVENT_KEYDOWN:
				if(e.key == 27) active=false;
				if((e.key == 'a' || e.key == 'j') && key_up == true) {
					snap = true;
					key_up = false;
					if(e.key == 'j') image_mode = 1;
					else if(e.key == 'a') image_mode = 2;
				}

				switch(e.skey) {
				case cmx::event::K_DOWN:
					af_x.decSpeed();
					break;
				case cmx::event::K_UP:
					af_x.incSpeed();
					break;
				case cmx::event::K_LEFT:
					if(times_pos > 0) times_pos--;
					break;
				case cmx::event::K_RIGHT:
					if(times_pos < 2) times_pos++;
					break;
				case cmx::event::K_SPACE:
						if(morph_index == 0) {
							morph_index = 1;
							std::cout << "Morph Enabled.\n";
						}
					break;
				case cmx::event::K_RETURN:
					 if(morph_index > 0) {
							std::cout << "Morph Disabled..\n";
							morph_index = 0;
					}
					break;
				}

				switch(e.key) {
				case ';':
					shift_it = false;
					rand_it = !rand_it;
					break;
				case '1':
					rand_it = false;
					shift_it = !shift_it;
					break;
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
				case '.':
					af_blend2 = 8;
					break;
				case 'h':
					af_blend2 = 9;
					break;
				case '-':
					af_blend2 = 10;
					break;
				case '9':
					af_blend2 = 11;
					break;
				case '5':
					af_blend2 = 12;
					break;
				case '7':
					af_blend2 = 13;
					break;
				case 'n':
					af_blend2 = 14;
					break;
				case '[':
					std::cout << "Morphing on\n";
					morph_on = true;
					break;
				case ']':
					std::cout << "Morphing off\n";	
					morph_on = false;
					break;
				case ' ':
					af_blend2 = 15;
					break;
				case '\\':
					af_blend2 = 16;
					break;
				case '=':
					af_blend2 = 17;
					break;
						
				case 'i':
						checkInput(back_surface);
						break;
				}
				break;
			case cmx::event::EVENT_KEYUP:

				if(e.key == 'a' || e.key == 'j') key_up = true;

				break
				;
			default:
				break;
			}
		}
		unsigned int *buffer = (unsigned int *) surface->buffer;
		if(slow == false ) {

			
			if(shift_it == true) {
				af_blend2 ++;
				if(af_blend2 > 7) af_blend2 = 0;
			}
			else if(rand_it == true) {
				af_blend2 = rand()%8;
			}

			af_x.Morph(back_surface);
			if(morph_on == true) {
				if(morph_index > 0 && morph_index < 7) {
					af *a = &af_x;
					(*a.*members[morph_index])(back_surface);
					morph_index ++;
					if(morph_index >= 7) morph_index = 1;
				}
			}
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
					af_x.Practice(back_surface, times_pos);
					break;
				case 8:
					af_x.RGBMix(back_surface);
					break;
				case 9:
					af_x.Perfect(back_surface);
					af_x.TriMorph(back_surface);
					break;
				case 10:
					af_x.GenEnergy(back_surface);
					af_x.TriMorph(back_surface);
					break;
				case 11:
					af_x.G(back_surface);
					break;
				case 12:
					af_x.Depressed(back_surface);
					break;
				case 13:
					af_x.TriBlendParadox(back_surface);
					break;
				case 14:
					af_x.ThoughtThis(back_surface);
					break;

				case 15:
					af_x.TriangleShift(af_x.image_files[rand()%4].the_surface, back_surface);
					af_x.Label(back_surface);
					break;
				case 16:
					af_x.Stoned(back_surface);
					break;
				case 17:
					af_x.Used(back_surface);
					break;
			}
		}
		surface->copySurface(*back_surface, back_surface->objectRect(), Rect(Point(0,0), back_surface->objectSize()));
		static float alpha = 0.1f;
		if(red_blue_b == false)
		copy_surface(*surface);
		else {
			static int cnt = 0;

			switch(cnt) {
			case 0:
				af_x.TransTo(surface, red_blue);
				copy_surface(*red_blue);
				break;
			case 1:
				af_x.TransToColor(surface, redcolor, 0);
				af_x.TransToColor(surface, bluecolor, 1);
				af_x.AlphaBlend(bluecolor, redcolor, alpha);
				copy_surface(*bluecolor);
				break;
			}
			cnt = !cnt;
			alpha += 0.001f;
		}
		if(snap == true) {
			the_font.printString((unsigned int*)surface->buffer, surface->w, 25, 25, 30, 25, 0xFFFFFFFF, "LostSideDead.com - AlphaFlameHD");
			snap = false;
			std::ostringstream stream;
			static unsigned int i=0;
			++i;
			std::string addstr = "";
			std::string ext = ((image_mode == 1) ? ".jpg" : ".png");
			if(prefix_str != 0) addstr = std::string(prefix_str) + "_";
			stream << addstr <<  "alphaflame_" << i << ext;
			copy_surface(*surface);
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

inline void checkInput(cmx::video::Surface *back_surface) {
	af_x.Morph(back_surface);
	if(morph_on == true) {
		if(morph_index > 0 && morph_index < 7) {
			af *a = &af_x;
			(*a.*members[morph_index])(back_surface);
			morph_index ++;
			if(morph_index >= 7) morph_index = 1;
		}
    }
	
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
			af_x.Practice(back_surface, times_pos);
			break;
		case 8:
			af_x.RGBMix(back_surface);
			break;
		case 9:
			af_x.Perfect(back_surface);
			af_x.TriMorph(back_surface);
			break;
		case 10:
			af_x.GenEnergy(back_surface);
			af_x.TriMorph(back_surface);
			break;
		case 11:
			af_x.G(back_surface);
			break;
		case 12:
			af_x.Depressed(back_surface);
			break;
		case 13:
			af_x.TriBlendParadox(back_surface);
			break;
		case 14:
			af_x.ThoughtThis(back_surface);
			break;
		case 15:
			af_x.TriangleShift(af_x.image_files[rand()%4].the_surface, back_surface);
			af_x.Label(back_surface);
			break;
		case 16:
			af_x.Stoned(back_surface);
			break;
		case 17:
			af_x.Used(back_surface);
			break;
	}
}
