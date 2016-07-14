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


#include"SDL.h"
#include<cmx.h>
#include<cmx_gl.h>
#if __APPLE__
#include</System/Library/Frameworks/OpenGL.framework/Headers/gl.h>
#include</System/Library/Frameworks/OpenGL.framework/Headers/glu.h>
#else

#include<windows.h>
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
bool mode_switch = false;
static unsigned long frame_count = 0, frame_count_change = 0;

void copy_surface(cmx::video::Surface &surf) {
	cmx::gl::draw_surface(&surf);
	cmx::gl::swap_gl_buffers();
}

typedef void (af::*func_ptr)(cmx::video::Surface *);
static func_ptr members[] = { &af::blendImage2, &af::TriMorph, 
	&af::ScrambleMe, &af::SquareBlend, &af::RGBMix, &af::G,
	&af::TriBlendParadox, &af::ThoughtThis
};

std::string openFileName() {
    OPENFILENAME ofn;       // common dialog box structure
    std::string fresult;
    static char szFile[256];
    
   
    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = GetForegroundWindow();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "jpg\0*.jpg\0png\0*.png\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    
    if (GetOpenFileName(&ofn)==TRUE) 
    {
        fresult = ofn.lpstrFile;
        return fresult;
    }
    return "";
}


int af_blend2 = 0;
af af_x;
bool grab_input = false;
void checkInput(cmx::video::Surface *);

void frames() {
	std::cout << "filter changed, frames generated " <<  frame_count_change << "\n";
	frame_count_change = 0;
}


int main(int argc, char **argv) {
	cmx::font::Font the_font;
	srand( (unsigned int) time(0) );
	int opt;
	int mode = 720;
	bool full = false, slow = false;
	int image_mode = 1;
	const char *file_str = 0, *prefix_str = 0, *path_str = 0;
	while ((opt = getopt(argc, argv, "dfsi:p:e:m")) != -1) {
		  switch(opt) {
               case 'd':
                  grab_input = true;
                  break;
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

	if(file_str == 0 && grab_input == false) {
		std::cout << argv[0] << " " << CURRENT_AFHD << ": usage\n-i source file\n-f fullscreen\n-s slow mode\n-p snapshot prefix\n-e set current image path\n-m 1080p mode\n\n";
		exit(0);
	} else  if(grab_input == true) {
        
        std::string fname1[4];
        
        for(int i = 0; i < 4; ++i) {
        
            fname1[i] = openFileName();
            if(fname1[i] == "") { MessageBox(0,"Try again ", "Ok", MB_ICONINFORMATION|MB_OK); return -1; }
        }
        std::fstream file;
        file.open("output.txt",std::ios::out);
        
        for(int i = 0; i < 4; ++i) {
            
            if(fname1[i] == "") { MessageBox(0, "try again to select 4 images", "try?", MB_ICONINFORMATION | MB_OK);
                return 1;
            }
            file << fname1[i] << "\n";
        }
        file.close();
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
	if(grab_input == false) af_x.buildImages(file_str,cmx::system::width, cmx::system::height);
    else af_x.buildImages("output.txt", cmx::system::width, cmx::system::height);
    
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
					af_x.ShiftImages();
					break;
				case cmx::event::K_RIGHT:
					if(times_pos < 2) times_pos++;
					af_x.ShiftImages();
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
					case '3':
						red_blue_b = true;
						break;
					case '0':
						red_blue_b = false;
						break;
					case 'r':
						mode_switch = !mode_switch;
						break;
					case 's':
						af_x.fade = af::FADE_RAND;
						break;

					case 'x':
						frames();
						std::cout << "Reset Variables\n";
						af_x.resetAlpha();
						break;
					case 'z':
						frames();
						std::cout << "Blend Image 2\n";
						af_blend2 = 1;
						break;
					case 'o':
						frames();
						std::cout << "Blend Image\n";
						af_blend2 = 0;
						break;
					case 't':
						frames();
						std::cout << "TriMorph\n";
						af_blend2 = 2;
						break;
					case 'm':
						frames();
						std::cout << "ScrambleME\n";
						af_blend2 = 3;
						break;
					case 'u':
						frames();
						std::cout << "blend/trimoprh/scramble me\n";
						af_blend2 = 4;
						break;
					case 'w':
						frames();
						std::cout << "Triangle Shift\n";
						af_blend2 = 5;
						break;
					case 'q':
						frames();
						std::cout << "Square Blend\n";
						af_blend2 = 6;
						break;
					case 'l':
						frames();
						std::cout << "Practice\n";
						af_blend2 = 7;
						break;
					case '.':
						frames();
						std::cout << "RGBMix\n";
						af_blend2 = 8;
						break;
					case 'h':
						frames();
						std::cout << "Perfect TriMorph\n";
						af_blend2 = 9;
						break;
					case '-':
						frames();
						std::cout << "GenEnergy TriMoprh\n";
						af_blend2 = 10;
						break;
					case '9':
						frames();
						std::cout << "G\n";
						af_blend2 = 11;
						break;
					case '5':
						frames();
						std::cout << "Depressed\n";
						af_blend2 = 12;
						break;
					case '7':
						frames();
						std::cout << "TriBlendParadox\n";
						af_blend2 = 13;
						break;
					case 'n':
						frames();
						std::cout << "Thought This\n";
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
					case 'g':
						frames();
						std::cout << "Triangle Shift Label\n";
						af_blend2 = 15;
						break;
					case '8':
						frames();
						std::cout << "Stoned\n";
						af_blend2 = 16;
						break;
					case '=':
						frames();
						std::cout << "Used\n";
						af_blend2 = 17;
						break;
					case 'i':
						checkInput(back_surface);
						break;
					case '1':
						af_x.setNegative(true);
						break;
					case 'c':
						af_x.setNegative(false);
						break;
					case '\\':
						af_x.setOpposite(true);
						break;
					case '/':
							af_x.setOpposite(false);
							break;
					case '4':
							frames();
							std::cout << "Test Code\n";
							af_blend2 = 18;
							break;
					case 'p':
							frames();
							std::cout << "ColorBlend\n";
							af_blend2 = 19;
							break;
					case 'b':
							frames();
							std::cout << "MirrorBlend\n";
							af_blend2 = 20;
							break;
				    case 'f':
							frames();
							std::cout << "Shift Pixels\n";
							af_blend2 = 21;
							break;

					default:
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

			e.key = 0;
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
			checkInput(back_surface);
		}
		surface->copySurface(*back_surface, back_surface->objectRect(), Rect(Point(0,0), back_surface->objectSize()));
		++frame_count;
		++frame_count_change;
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
			the_font.printString((unsigned int*)surface->buffer, surface->w, 25, 25, 30, 25, _RGB(255,255,255), "LostSideDead.com - AlphaFlameHD");
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
    std::cout << "Program stopped, " << frame_count << " frames generated.\n";
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
			af_x.blendImage(back_surface, af_x.image_files[ (mode_switch == true) ? mode : rand()%4].the_surface);
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
        case 18:
            af_x.TestCode(back_surface);
            break;
        case 19:
            af_x.ColorBlend(back_surface);
            break;
        case 20:
            af_x.MirrorBlend(back_surface);
            break;
        case 21:
        	af_x.ShiftPixels(back_surface);
        	break;
	}
}
