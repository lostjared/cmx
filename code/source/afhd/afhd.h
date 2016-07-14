/*
 *
 * AlphaFlame HD (Distortion Algorithms)
 * written by Jared Bruni
 * (C) 2011 LostSideDead
 *
 * http://lostsidedead.com
 * License: GPL
 *
 */

#ifndef __AF_HD_H_
#define __AF_HD_H_
#define CURRENT_AFHD 2.25
#include<cmx.h>
#include<iostream>
#include<fstream>
#include<vector>
#include<cmath>

static bool is_negative, is_opposite;
static int mode=0;


#ifdef _RGB
#undef _RGB
inline unsigned int _RGB(unsigned char r, unsigned char g, unsigned char b) {
	if(is_negative == true) return cmx::CMX_RGB((r+1)%255,(g+1)%255,(b+1)%255).color.color_value;
	if(is_opposite == true) return cmx::CMX_RGB(b, g, r).color.color_value;
	return cmx::CMX_RGB(r,g,b).color.color_value;
}
#endif


	class afImage {
	public:
		afImage() {
			the_surface = 0;
		}
		~afImage() {
			if(the_surface != 0) delete the_surface;
		}

		bool loadImage(std::string s, int nw, int nh) {
			cmx::video::Surface surf;
			if(surf.loadImage(s.c_str())) {
				the_surface=cmx::video::newSurface(nw, nh);
				unsigned int *src_ptr = (unsigned int *)surf.buffer;
				unsigned int *dst_ptr = (unsigned int *)the_surface->buffer;

				for(int i = 0; i < nw; ++i) {
					for(int z = 0; z < nh; ++z) {
						int pos_x=CMX_GetFW(surf.w, i, nw);
						int pos_y=CMX_GetFH(surf.h, z, nh);
						dst_ptr[i+z*nw] = src_ptr[pos_x+(pos_y*surf.w)];
					}
				}
			} else {
				std::cerr << "Error could not load image: " << s << "\n";
				exit(0);
				return false;
			}
			return true;
		}

		cmx::video::Surface *the_surface;
	};

	class af {
	public:
		afImage image_files[4];

		af() {
			resetAlpha();
			is_negative = false;
			is_opposite = false;
		}

		void resetAlpha() {
			tri_alpha = 1.0f;
			morph_alpha = 1.0f;
			blend_alpha = 1.0f;
			b_alpha = 1.0f;
			trans_speed = 0.1f;
			square_alpha = 1.0f;
			lame_alpha = 1.0f;
			circle_alpha = 1.0f;
			rgb_alpha = 1.0f;
			falpha = 1.0f;
			alpha_per = 1.0f;
			v_a[0] = v_a[1] = v_a[2] = 1.0f;
			g_fv[0] = g_fv[1] = g_fv[2] = 0;
		}

		void ShiftImages() { ++mode; if(mode > 3) mode = 0; }
		void incSpeed() { trans_speed += 0.1f; }
		void decSpeed() { trans_speed -= 0.1f; }
		void setNegative(bool neg_value) {
			is_negative = neg_value;
			std::cout << "Negative value changed: " << ((is_negative==true)?"true":"false") << "\n";
		}
		void setOpposite(bool opp_value) {
			is_opposite = opp_value;
			std::cout << "Opposite value: " << ((is_opposite==true)?"true":"false") << "\n";
		}

		enum FADE { FADE_R, FADE_G, FADE_B, FADE_RAND };
		FADE fade;
		float trans_speed;
		float square_alpha;
		float lame_alpha;
		float circle_alpha;
		float rgb_alpha;
		float falpha;
		float alpha_per;
		float g_fv[3];

		void buildImages(std::string list_file, int nw,  int nh) {
			std::fstream file;
			file.open(list_file.c_str(), std::ios::in);
			if(!file.is_open()) {
				std::cerr << "Error opening list file: " << list_file << "\n";
				exit(0);
			}
			std::vector<std::string> images;
			while(!file.eof()) {
				std::string str;
				std::getline(file, str);
				images.push_back(str);
			}
			file.close();
			if(images.size()<4) {
				std::cerr << "Error four images are required\n";
				exit(0);
			}
			for(int i = 0; i < 4; i++) {
				if(!image_files[i].loadImage(images[i],nw, nh)) {
					std::cerr << "could not load file: " << images[i] << "\n";
					exit(0);
				}
			}
		}

		inline void getPixel(int x, int y, cmx::video::Surface *s, cmx::Color &col) {
			col.color.color_value = ((unsigned int*)s->buffer)[x+y*s->w];
		}

		void buildStartImage(cmx::video::Surface *surface) {
			unsigned int *b = (unsigned int *)surface->buffer;
			for(int i =  0; i < surface->w; ++i) {
				for(int z = 0; z < surface->h; ++z) {
					cmx::Color col1;
					cmx::Color col2;
					cmx::Color col3;
					getPixel(i,z,image_files[rand()%4].the_surface, col1);
					getPixel(i,z,image_files[rand()%4].the_surface, col2);
					getPixel(i,z,image_files[rand()%4].the_surface, col3);
					b[i+z*surface->w] = _RGB(col1.color.color[0], col2.color.color[1], col3.color.color[2]);
				}
			}
		}

		float blend_alpha;

		void blendImage2(cmx::video::Surface *back_surface) {
			blend_alpha += trans_speed;
			unsigned int *buffer = (unsigned int*)back_surface->buffer;
			int i,z;
			for(i = 0; i < back_surface->w; i++) {
				for(z = 0; z < back_surface->h; z++) {
					cmx::Color start_color, col1, col2, col3;
					getPixel(i,z,back_surface, start_color);
					getPixel(i,z, image_files[0].the_surface, col1);
					getPixel(i,z, image_files[1].the_surface, col2);
					getPixel(i,z, image_files[2].the_surface, col3);
					start_color.color.color[0] += (unsigned char)(1-blend_alpha) * col1.color.color[0];
					start_color.color.color[1] += (unsigned char)(1-blend_alpha) * col2.color.color[1];
					start_color.color.color[2] += (unsigned char)(1-blend_alpha) * col3.color.color[2];
					buffer[i+z*back_surface->w] =  _RGB(start_color.color.color[0], start_color.color.color[1], start_color.color.color[2]);
				}
			}
		}

		float b_alpha ;

		void blendImage(cmx::video::Surface *back_surface, cmx::video::Surface *blend_image) {
			unsigned int *back_buffer = (unsigned int *)back_surface->buffer;
			for(int i = 0; i < back_surface->w; ++i) {
				for(int z = 0; z < back_surface->h; ++z) {
					cmx::Color color, color_image, fcolor;
					getPixel(i, z, blend_image, color);
					getPixel(i, z, back_surface, color_image);
					color_image.color.color[0] += (unsigned char)(b_alpha * color.color.color[0]);
					color_image.color.color[1] += (unsigned char)(b_alpha * color.color.color[1]);
					color_image.color.color[2] += (unsigned char)(b_alpha * color.color.color[2]);
					back_buffer[ i + z * back_surface->w ] =  _RGB(color_image.color.color[0], color_image.color.color[1], color_image.color.color[2]);
				}
			}
			b_alpha += trans_speed;
		}

		void Morph(cmx::video::Surface *surface) {
			static unsigned int i,z;
			unsigned int *b = (unsigned int *)surface->buffer;
			for(i = 0; i < surface->w-2; i++) {
			          for(z = 0; z <surface->h-2; z++) {
			        	  cmx::Color colors[8];
			        	  getPixel(i, z, surface, colors[0]);
			        	  getPixel(i+1, z, surface,colors[1]);
			        	  getPixel(i, z+1, surface, colors[2]);
			        	  int col[3];
			        	  col[0] = colors[0].color.color[0] + colors[1].color.color[0] + colors[2].color.color[0];
			        	  col[1] = colors[0].color.color[1] + colors[1].color.color[1] + colors[2].color.color[1];
			        	  col[2] = colors[rand()%4].color.color[0] + colors[rand()%3].color.color[2] + colors[rand()%4].color.color[2];
			        	  cmx::Color fcolor;
			        	  col[0] /= 4;
			        	  col[1] /= 4;
			        	  col[2] /= 4;
			        	  switch(fade) {
			        	  case FADE_R:
			        		  fcolor.color.color[0] = ++col[0];
			        		  fcolor.color.color[1] = --col[1];
			        		  fcolor.color.color[2] = --col[2];
			        		  break;
			        	  case FADE_G:
			        		  fcolor.color.color[0] = --col[0];
			        		  fcolor.color.color[1] = ++col[1];
			        		  fcolor.color.color[2] = --col[2];
			        		  break;
			        	  case FADE_B:
			        		  fcolor.color.color[0] = --col[0];
			        		  fcolor.color.color[1] = --col[1];
			        		  fcolor.color.color[2] = ++col[2];
			        		  break;
			        	  case FADE_RAND:
			        		  fcolor.color.color[0] = ++col[rand()%3];
			        		  fcolor.color.color[1] = ++col[rand()%3];
			        		  fcolor.color.color[2] = ++col[rand()%3];
			        		  break;
			        	  }
			        	  b[i+z*surface->w] = _RGB(fcolor.color.color[0], fcolor.color.color[1], fcolor.color.color[2]);
			     }
			}

		}

		float tri_alpha;

		void TriMorph(cmx::video::Surface *surface) {
			unsigned int *ptr = (unsigned int*) surface->buffer;
			static int i=0,z=0;
			for(i = 10; i < surface->w-10; ++i) {
				for(z = 10; z < surface->h-10; ++z) {
					cmx::Color col1,col2,col3,start;
					getPixel(i, z, surface, start);
					getPixel(i+3, z+1, image_files[0].the_surface, col1);
					getPixel(i+1, z, image_files[1].the_surface, col2);
					getPixel(i+4, z, image_files[2].the_surface, col3);
					start.color.color[0] += (unsigned char)(1-tri_alpha)*col1.color.color[2];
					start.color.color[1] += (unsigned char)(1-tri_alpha)*col2.color.color[1];
					start.color.color[2] += (unsigned char)(1-tri_alpha)*col3.color.color[0];
					ptr[i+z*surface->w] = _RGB(start.color.color[0], start.color.color[1], start.color.color[2]);
				}
			}
			tri_alpha += trans_speed;
		}

		float morph_alpha;

		void ScrambleMe(cmx::video::Surface *surface) {
			unsigned int *lost = (unsigned int *) surface->buffer;
			static int counter=0;
			for(int i = 0; i < surface->w; ++i) {
				for(int z = 0; z < surface->h; ++z) {
					cmx::Color col1,col2,col3,side;
					getPixel(i, z, surface, side);
					getPixel(i, z, image_files[0].the_surface, col1);
					getPixel(i, z, image_files[1].the_surface, col2);
					getPixel(i, z, image_files[2].the_surface, col3);
					side.color.color[0] += (unsigned char)(1-morph_alpha) * col1.color.color[counter];
					side.color.color[1] += (unsigned char)(1-morph_alpha) * col2.color.color[counter];
					side.color.color[2] += (unsigned char)(1-morph_alpha) * col3.color.color[counter];
					lost[i+z*surface->w] =  _RGB(side.color.color[0], side.color.color[1], side.color.color[2]);
				}
			}
			morph_alpha += trans_speed;
			++counter;
			if(counter > 3) counter = 0;
		}

		void TransTo(cmx::video::Surface *back, cmx::video::Surface *dest) {
			static bool w = true;
			w = !w;
			unsigned int *buffer = (unsigned int *)dest->buffer;
			for(int z=0; z < dest->h; ++z) {
				for(int i=0; i < dest->w; ++i) {
					cmx::Color col;
					getPixel(i, z, back, col);
					if(w) buffer[i+z*dest->w] = _RGB(255, col.color.color[1], col.color.color[2]);
					else  buffer[i+z*dest->w] = _RGB(col.color.color[0],col.color.color[1], 255);
					static int cnt = 0;
					if((++cnt%3) == 0) w = !w;
				}
			}
		}


		void TransToColor(cmx::video::Surface *back, cmx::video::Surface *where, int clr) {
			unsigned int *buffer = (unsigned int *) where->buffer;
			for(int z = 0; z < where->h; ++z) {
				for(int i=0; i< where->w; ++i) {
					cmx::Color col;
					getPixel(i, z, back, col);
					if(clr == 0)
					buffer[i+z*where->w] = _RGB(col.color.color[0], 0, 0);
					else
					buffer[i+z*where->w] = _RGB(0, 0, col.color.color[2]);
				}
			}
		}

		void AlphaBlend(cmx::video::Surface *back, cmx::video::Surface *blend, float alpha) {
			unsigned int *buffer = (unsigned int *)back->buffer;
			for(int z = 0; z < back->h; ++z) {
				for(int i = 0; i < back->w; ++i) {
					cmx::Color src, add;
					getPixel(i, z, back, src);
					getPixel(i, z, blend, add);
					src.color.color[0] += (unsigned char)(1-alpha) * add.color.color[0];
					src.color.color[1] += (unsigned char)(1-alpha) * add.color.color[1];
					src.color.color[2] += (unsigned char)(1-alpha) * add.color.color[2];
					buffer[i+z*back->w] = _RGB(src.color.color[0], src.color.color[1], src.color.color[2]);
				}
			}
		}

		void TriangleShift(cmx::video::Surface *back, cmx::video::Surface *dest) {
			unsigned int *buffer = (unsigned int *)dest->buffer;
			for(int z = 0; z < dest->h-2; ++z) {
				for(int i = 0; i < dest->w-6; ++i) {
					cmx::Color the_color[3], final_color;
					getPixel(i+1, z+1, back, the_color[0]);
					buffer[i+z*dest->w] = _RGB(the_color[0].color.color[0], the_color[0].color.color[1], the_color[0].color.color[2]);
				}
			}
		}

		void SquareBlend(cmx::video::Surface *surface) {
			unsigned int *buffer = (unsigned int*)surface->buffer;
			static int offset = 0;
			for(int z = 0; z < surface->h-1; ++z) {
				for(int i = 0; i < surface->w-1; ++i) {
					cmx::Color col1,col2,col3;
					getPixel(i, z, surface, col3);
					getPixel(surface->w-i-1, surface->h-z-1, image_files[offset].the_surface,col1);
					getPixel(surface->w-i-1, surface->h-z-1, image_files[0].the_surface,col2);
					col3.color.color[0] += (unsigned char)(1-square_alpha) * col1.color.color[0];
					col3.color.color[1] += (unsigned char)(1-square_alpha) * col2.color.color[1];
					col3.color.color[2] += (unsigned char)(1-square_alpha) * col1.color.color[2]-col2.color.color[2];
					buffer[i+z*surface->w] = _RGB(col3.color.color[0], col3.color.color[1], col3.color.color[2]);
				}
			}
			++offset;
			if(offset > 3) offset = 0;
			square_alpha += trans_speed;
		}

		void Lame(cmx::video::Surface *surface) {
			unsigned int *buffer = (unsigned int *)surface->buffer;
			for(int z = 0; z  < surface->h; ++z) {
				for(int i = 0; i < surface->w; ++i) {
					cmx::Color fcolor;
					getPixel(i, z, surface, fcolor);
					fcolor.color.color[0]++;
					fcolor.color.color[1]--;
					fcolor.color.color[2]++;
					buffer[i+z*surface->w] = _RGB(fcolor.color.color[0], fcolor.color.color[1], fcolor.color.color[2]);
				}
			}
			lame_alpha += trans_speed;
		}

		void CircleDraw(cmx::video::Surface *surface) {
			int r = 5;
			int s = 0;
			unsigned int *buffer = (unsigned int*)surface->buffer;
			for(int z = 0; z < surface->h; ++z) {
				for(int i = 0; i < surface->w; ++i) {
					float theta = 2.0f * 3.1415926f * float(i) / float(5);
					int x = int(r*cosf(theta));
					int y = int(r*sinf(theta));
					cmx::Color color;
					cmx::Color colors[4];
					getPixel(i,z,surface,color);
					if(x < surface->w && y < surface->h) {
						getPixel(x,y,image_files[0].the_surface, colors[0]);
						getPixel(x,y,image_files[1].the_surface, colors[1]);
						getPixel(x,y,image_files[2].the_surface, colors[2]);
					}
					color.color.color[0] += (unsigned char)(1-circle_alpha) * colors[0].color.color[0];
					color.color.color[1] += (unsigned char)(1-circle_alpha) * colors[1].color.color[1];
					color.color.color[2] += (unsigned char)(1-circle_alpha) * colors[2].color.color[2];
					++s;
					if(s > 3) s = 0;
					buffer[i+z*surface->w] = _RGB(color.color.color[0], color.color.color[1], color.color.color[2]);
				}
			}
			circle_alpha += trans_speed;
		}

		void RGBMix(cmx::video::Surface *surface) {
			unsigned int *buffer = (unsigned int *)surface->buffer;
			cmx::Color colors[5];
			int col = 0;
			static int dir = 1;
			for(int z = 0; z < surface->h; ++z) {
				for(int i = 0; i < surface->w; ++i) {
					getPixel(i, z, image_files[0].the_surface, colors[0]);
					getPixel(i, z, image_files[1].the_surface, colors[1]);
					getPixel(i, z, image_files[2].the_surface, colors[2]);
					getPixel(i, z, image_files[3].the_surface, colors[3]);
					getPixel(i, z, surface, colors[4]);
					col = 0;
					colors[4].color.color[0] += (unsigned char)(1-rgb_alpha) * colors[(dir == 1) ? col : 3-col].color.color[0];
					col++;
					colors[4].color.color[1] += (unsigned char)(1-rgb_alpha) * colors[(dir == 1) ? col : 3-col].color.color[1];
					col++;
					colors[4].color.color[2] += (unsigned char)(1-rgb_alpha) * colors[(dir == 1) ? col : 3-col].color.color[2];
					buffer[i+z*surface->w] = _RGB(colors[4].color.color[0], colors[4].color.color[1], colors[4].color.color[2]);
				}
			}
			rgb_alpha += trans_speed;
			dir = !dir;
		}


		void Practice(cmx::video::Surface *surface, int times) {
			static int shift_ = 0;
			unsigned int *buffer = (unsigned int *)surface->buffer;
			for(int z = 0; z < surface->h; ++z) {
				for(int i = 0; i < surface->w; ++i) {
					cmx::Color colors[6];
					getPixel(i, z, surface, colors[0]);
					getPixel(i, z, image_files[0].the_surface, colors[1]);
					getPixel(i, z, image_files[1].the_surface, colors[2]);
					getPixel(i, z, image_files[2].the_surface, colors[3]);
					getPixel(i, z, image_files[3].the_surface, colors[4]);
					colors[5].color.color[0] = colors[1].color.color[0];
					colors[5].color.color[1] = colors[1].color.color[1]+colors[2].color.color[1];
					colors[5].color.color[2] = colors[1].color.color[2]+colors[2].color.color[2]+colors[3].color.color[2];
					colors[5].color.color[0] += (unsigned char)falpha * colors[shift_].color.color[0];
					colors[5].color.color[1] += (unsigned char)falpha * colors[shift_].color.color[1];
					colors[5].color.color[2] += (unsigned char)falpha * colors[shift_].color.color[2];
					buffer[i+z*surface->w] = _RGB(colors[5].color.color[0], colors[5].color.color[1], colors[5].color.color[2]);
					if(times == 0 && (++shift_) > 4) shift_ = 0;
				}
				if(times == 1 && (++shift_) > 4) shift_ = 0;
			}
			falpha += trans_speed;
			if(++shift_ > 4) shift_ = 0;
		}

		void Perfect(cmx::video::Surface *surface) {
			static int i,z;
			unsigned int *buffer = (unsigned int*)surface->buffer;
			static cmx::Color colors[8];
			for(z = 0; z < surface->h; ++z) {
				for(i = 0; i < surface->w; ++i) {
					getPixel(i, z, surface, colors[0]);
					getPixel(i, z, image_files[0].the_surface, colors[1]);
					getPixel(i, z, image_files[1].the_surface, colors[2]);
					getPixel(i, z, image_files[2].the_surface, colors[3]);
					colors[0][0] += (unsigned char) (colors[1][0] + colors[1][1]) * alpha_per;
					colors[0][1] += (unsigned char) (colors[1][0] - colors[1][1] + colors[2][2]) * alpha_per;
					colors[0][2] += (unsigned char) (colors[1][0] + colors[1][1] - colors[2][2] + colors[3][2]) * alpha_per;
					colors[0][0] /= 2;
					colors[0][1] /= 2;
					colors[0][2] /= 2;
					buffer[i+z*surface->w] = _RGB(colors[0][0], colors[0][1], colors[0][2]);
				}
			}
			alpha_per += trans_speed;
		}

		float v_a[3];

		void GenEnergy(cmx::video::Surface *surface) {
			static int i,z;
			unsigned int *buffer = (unsigned int *)surface->buffer;
			static cmx::Color colors[6];
			int pos = 0;
			for(z = 0; z < surface->h; ++z) {
				for(i = 0; i < surface->w; ++i) {
					int i_x=0, i_y=0;
					getPixel(i, z, surface, colors[0]);
					getPixel(i, z, image_files[0].the_surface, colors[1]);
					getPixel(i, z, image_files[1].the_surface, colors[2]);
					getPixel(i, z, image_files[2].the_surface, colors[3]);
					colors[4] = buffer[pos++];
					colors[0][0] += (unsigned char)colors[0][0] * v_a[0];
					colors[0][1] += (unsigned char)colors[1][1] * v_a[1];
					colors[0][2] += (unsigned char)colors[2][2] * v_a[rand()%2];
				}
			}
			v_a[0] += trans_speed;
			v_a[1] -= trans_speed;
			v_a[2] += trans_speed;
		}

		void G(cmx::video::Surface *surface) {
			static int i,z,j;
			static float inc_direction[3] = { -0.1f, 0.1f, -0.1f };;
			unsigned int *buffer = (unsigned int *)surface->buffer;
			cmx::Color colors[4];
			for(z = 0; z < surface->h; ++z) {
				for(i = 0; i < surface->w; ++i) {
					getPixel(i,z,image_files[0].the_surface, colors[0]);
					getPixel(i,z,image_files[1].the_surface, colors[1]);
					getPixel(i,z,image_files[2].the_surface, colors[2]);
					colors[0][0] += colors[0][0] * g_fv[0];
					colors[0][1] += colors[0][1]+colors[1][1] * g_fv[1];
					colors[0][2] += colors[0][2]+colors[1][2]+colors[2][2] * g_fv[2];
					buffer[i+z*surface->w] = colors[0].color.color_value;
				}
			}
			for(j = 0; j < 3; ++j) {
				if(g_fv[j] > 10) inc_direction[j] = -0.1;
				if(g_fv[j] < -10) inc_direction[j] = 0.1;
				g_fv[j] += inc_direction[j];
			}
		}

		void Depressed(cmx::video::Surface *surface) {

			static int i,z,j, depth = 1;;
			static float inc_direction[3] = { 0.1f, -0.1f, 0.1f };
			static float alpha_values[3] = { 1.0f, 1.0f, 1.0f };
			cmx::Color colors[4];
			unsigned int *buffer = (unsigned int*)surface->buffer;
			for(z = 0; z < surface->h-2; ++z) {
				for(i = 0; i < surface->w-2; ++i) {
					getPixel(i,z, surface, colors[0]);
					getPixel(i+1,z, image_files[0].the_surface, colors[1]);
					getPixel(i+1,z+1, image_files[1].the_surface,colors[2]);
					getPixel(i+1,z+2, image_files[2].the_surface, colors[3]);
					colors[0][0] += (colors[1][0] - colors[2][0]) * alpha_values[j];
					colors[0][1] += (colors[1][1] + colors[2][1]) * alpha_values[j];
					colors[0][2] += (colors[1][2] - colors[2][2]) * alpha_values[j];
					colors[0][3] = 255;
					buffer[i+z*surface->w] =  (is_negative == true) ? -colors[0].color.color_value : colors[0].color.color_value;
				}
			}
			for (j = 0; j < 3; ++j) {
				alpha_values[j] += inc_direction[j];
				if (alpha_values[j] < -20)
					inc_direction[j] = 0.1f;
				else if (alpha_values[j] > 20)
					inc_direction[j] = -0.1f;
			}
		}

		void TriBlendParadox(cmx::video::Surface *surface) {
			static int i=0,z=0;
			unsigned int *buffer = (unsigned int*)surface->buffer;
			cmx::Color colors[4];
			for(z = 0; z < surface->h-2; ++z) {
				for(i = 0; i < surface->w-2; ++i) {
					getPixel(i,z, image_files[0].the_surface, colors[0]);
					getPixel(i+1,z, image_files[1].the_surface, colors[1]);
					getPixel(i, z+1, image_files[2].the_surface, colors[2]);
					getPixel(i, z, surface, colors[3]);
					switch(rand()%3) {
						case 0:
							colors[3][0] += (colors[0][0]) * tri_alpha;
							colors[3][1] += (colors[0][1] + colors[1][1]) * tri_alpha;
							colors[3][2] += (colors[0][2] + colors[1][2] + colors[2][2]) * tri_alpha;
							break;
						case 1:
							colors[3][0] += (colors[0][2] + colors[1][2] + colors[2][2]) * tri_alpha;
							colors[3][1] += (colors[0][1] + colors[1][1]) * tri_alpha;
							colors[3][2] += (colors[0][0]*tri_alpha);
							break;
						case 2:
							colors[3][0] -= (colors[0][1]+colors[1][1]) * tri_alpha;
							colors[3][1] -= (colors[0][0]*tri_alpha);
							colors[3][2] -= (colors[0][2]+colors[1][2]+colors[2][2])*tri_alpha;
							break;
					}
					
					colors[3][3] = 0;
					buffer[i+z*surface->w] = (is_negative == true) ? -colors[3].color.color_value : colors[3].color.color_value;
				}
			}
			tri_alpha += trans_speed;
		}
		
		void ThoughtThis(cmx::video::Surface *surface) {
			static int i=0,z=0;
			unsigned int *buffer = (unsigned int*)surface->buffer;
			for(z = 0; z < surface->h-2; ++z) {
				for(i = 0; i < surface->w-2; ++i) {
					cmx::Color colors[3];
					cmx::video::Surface *surf = image_files[rand()%3].the_surface;
					getPixel(i,z, surf, colors[0]);
					getPixel(i+1,z, surf, colors[1]);
					getPixel(i, z+1, surf, colors[2]);
					colors[0][0] += (colors[0][0])*tri_alpha;
					colors[0][1] += (colors[0][0]+colors[1][1])*tri_alpha;
					colors[0][2] += (colors[0][0]+colors[1][1]+colors[2][2])*tri_alpha;
					buffer[i+z*surface->w] =  (is_negative == true) ? -colors[0].color.color_value : colors[0].color.color_value;

				}
				
			}
			tri_alpha += trans_speed;
		}

		void Label(cmx::video::Surface *surface) {
			static int i=0,z=0;
			int mode = 0;
			unsigned int *buffer = (unsigned int*)surface->buffer;
				for(z = 0; z < surface->h-2; ++z) {
					for(i = 0; i < surface->w-2; ++i) {
						cmx::Color colors[6];
						getPixel(surface->w-i, surface->h-z, surface, colors[0]);
						getPixel(i, z, surface, colors[1]);
						colors[0][0] += (1-tri_alpha) * colors[mode][0];
						colors[0][1] += (1-tri_alpha) * colors[mode][1];
						colors[0][2] += (1-tri_alpha) * colors[mode][2];
						if(mode > 1) mode = 0; else mode++;
						buffer[i+z*surface->w] = (is_negative == true) ? -colors[0].color.color_value : colors[0].color.color_value;
					}

				}
				tri_alpha += trans_speed;
		}


		void Stoned(cmx::video::Surface *surface) {
			static int i=0,z=0;
			int mode = 0;
			unsigned int *buffer = (unsigned int*)surface->buffer;
			for(z = 0; z < surface->h-2; ++z) {
				for(i = 0; i < surface->w-2; ++i) {
					cmx::Color colors[6];
					getPixel(i, z, surface, colors[0]);
					getPixel(i, z, image_files[0].the_surface, colors[1]);
					getPixel(i, z, image_files[1].the_surface, colors[2]);
					getPixel(i, z, image_files[2].the_surface, colors[3]);
					getPixel(i, z, image_files[3].the_surface, colors[4]);
					getPixel(i, z+1, surface, colors[5]);
					colors[5][0] += colors[mode][0] * tri_alpha;
					colors[5][1] += colors[mode][1] * tri_alpha;
					colors[5][2] += colors[mode][2] * tri_alpha;
					colors[5][3] = 255;
					mode++;
					if(mode > 5) mode = 0;
					int amount = 1;
					int val_r = 0, val_g = 0, val_b = 0;
					for(int q=0; q<5; ++q) {
						val_r += colors[q][0];
						val_g += colors[q][1];
						val_b += colors[q][2];
					}
					val_r /= mode+1;
					val_g /= mode+1;
					val_b /= mode+1;
					colors[5][0] +=  tri_alpha * (unsigned char)val_r;
					colors[5][1] +=  tri_alpha * (unsigned char)val_g;
					colors[5][2] +=  tri_alpha * (unsigned char)val_b;
					colors[5][3] = 255;
					buffer[i+z*surface->w] = (is_negative == true) ? -colors[5].color.color_value : colors[5].color.color_value;
				}
			}
			tri_alpha += trans_speed;
		}

		void Used(cmx::video::Surface *surface) {
			static int i=0,z=0;
			unsigned int *buffer = (unsigned int*)surface->buffer;
			for(z = 0; z < surface->h; ++z) {
				for(i = 0; i < surface->w; ++i) {
					cmx::Color color_value;
					cmx::Color colors[5];
					getPixel(i, z, surface, colors[0]);
					getPixel(i, z, image_files[0].the_surface, colors[1]);
					getPixel(i, z, image_files[1].the_surface, colors[2]);
					getPixel(i, z, image_files[2].the_surface, colors[3]);
					getPixel(i, z, image_files[3].the_surface, colors[4]);
					int val_r= colors[0][0] + colors[1][0] + colors[2][0] + colors[3][0] + colors[4][0] + 1;
					int val_g= colors[0][1] + colors[1][1] + colors[2][1] + colors[3][1] + colors[4][1] + 1;
					int val_b= colors[0][2] + colors[1][2] + colors[2][2] + colors[3][2] + colors[4][2] + 1;
					val_r /= 5;
					val_g /= 5;
					val_b /= 5;
					unsigned char vr  = ((unsigned char)(val_b%255)*tri_alpha);
					unsigned char vg  = ((unsigned char)(val_g%255)*tri_alpha);
					unsigned char vb  = ((unsigned char)(val_r%255)*tri_alpha);
					color_value[0] = vr;
					color_value[1] = vg;
					color_value[2] = vb;
					buffer[i+z*surface->w] = (is_negative == true) ? -color_value.color.color_value : color_value.color.color_value;
				}
			}

			tri_alpha += trans_speed;
		}

		// you have to reset this one quite often. call resetAlpha()
		void TestCode(cmx::video::Surface *surface) {
			static int i=0,z=0;
			unsigned int *buffer = (unsigned int*)surface->buffer;
			for(z = 0; z < surface->h; ++z) {
				for(i = 0; i < surface->w; ++i) {
					cmx::Color color_value;
					cmx::Color colors[5];
					getPixel(i, z, surface, colors[0]);
					getPixel(i, z, image_files[0].the_surface, colors[1]);
					getPixel(i, z, image_files[1].the_surface, colors[2]);
					getPixel(i, z, image_files[2].the_surface, colors[3]);
					getPixel(i, z, image_files[3].the_surface, colors[4]);
					color_value[0] = (colors[0][0]+colors[2][0])*tri_alpha;
					color_value[1] = (colors[0][1]+colors[1][1])*tri_alpha;
					color_value[2] = (colors[0][2]+colors[0][2])*tri_alpha;
					color_value[0] += colors[3][0];
					color_value[1] += colors[3][1];
					color_value[2] += colors[3][2];

					buffer[i+z*surface->w] = (is_negative == true) ? -color_value.color.color_value : color_value.color.color_value;
				}
			}
			tri_alpha += trans_speed;
		}

		void ColorBlend(cmx::video::Surface *surface) {
			static int i=0,z=0;
			unsigned int *buffer = (unsigned int*)surface->buffer;
			for(z = 0; z < surface->h; ++z) {
				for(i = 0; i < surface->w; ++i) {
					cmx::Color color_value;
					cmx::Color colors[3];
					getPixel(i, z, surface, colors[0]);
					getPixel(i, z, surface, color_value);
					getPixel(i, z, image_files[0].the_surface, colors[1]);
					getPixel(i, z, image_files[rand()%4].the_surface, colors[2]);
					color_value[0] = colors[1][0];//*tri_alpha;
					color_value[2] = colors[2][2]*(-tri_alpha);
					color_value[1] += colors[1][0]+colors[2][2]*tri_alpha;
					buffer[i+z*surface->w] = (is_negative == true) ? -color_value.color.color_value : color_value.color.color_value;
				}
			}
			tri_alpha += trans_speed;

		}
		void MirrorBlend(cmx::video::Surface *surface) {
			static int i=0,z=0;
			unsigned int *buffer = (unsigned int*)surface->buffer;
			for(z = 0; z < surface->h; ++z) {
				for(i = 0; i < surface->w; ++i) {
					cmx::Color color_value;
					cmx::Color colors[3];
					getPixel(i,z, surface, color_value);
					getPixel(surface->w-i, surface->h-z, surface, colors[0]);
					color_value[0] += colors[0][0]*tri_alpha;
					color_value[1] += colors[0][1]*tri_alpha;
					color_value[2] += colors[0][2]*tri_alpha;
					buffer[i+z*surface->w] = (is_negative == true) ? -color_value.color.color_value : color_value.color.color_value;
				}
			}
			tri_alpha += trans_speed;
		}

		void ShiftPixels(cmx::video::Surface *surface) {
			static int i=0,z=0;
			unsigned int *buffer = (unsigned int*)surface->buffer;
			for(z = 0; z < surface->h-2; ++z) {
				for(i = 0; i < surface->w-2; ++i) {
					cmx::Color color_value;
					cmx::Color color2, temp_color, color3;
					getPixel(i, z, surface, color_value);
					getPixel(i+1, z+1, surface, color2);
					getPixel(i, z, image_files[mode].the_surface, color3);
					temp_color = color_value;
					color_value = color2;
					color2 = temp_color;
					color_value[0] += color3[0]*tri_alpha;
					color_value[1] += color3[1]*tri_alpha;
					color_value[2] += color3[2]*tri_alpha;
					buffer[i+z*surface->w] = (is_negative == true) ? -color_value.color.color_value : color_value.color.color_value;
				}
			}
			tri_alpha += trans_speed;
			//++mode;
			if(mode > 3) mode = 0;
		}

		void Blend3 (cmx::video::Surface *surface) {
			static int i=0,z=0;
			static float rValue[3] = { 0, 0, 0 };
			unsigned int *buffer = (unsigned int*)surface->buffer;
			for(z = 0; z < surface->h; ++z) {
				for(i = 0; i < surface->w; ++i) {
					cmx::Color color_value;
					cmx::Color colors[3];
					getPixel(i,z, surface, color_value);
					getPixel(i,z, image_files[0].the_surface, colors[0]);
					getPixel(i,z, image_files[1].the_surface, colors[1]);
					getPixel(i,z, image_files[2].the_surface, colors[2]);
					float value[3];
					value[0] = colors[0][0]+colors[1][0]+colors[2][0];
					value[0] *= rValue[0];
					value[1] = colors[0][1]+colors[1][1]+colors[2][1];
					value[1] *= rValue[1];
					value[2] = colors[0][2]+colors[1][2]+colors[2][2];
					value[2] *= rValue[2];
					color_value[0] += static_cast<unsigned char>(value[0]);
					color_value[1] += static_cast<unsigned char>(value[1]);
					color_value[2] += static_cast<unsigned char>(value[2]);
					color_value[3] = 0xFF;
					buffer[i+z*surface->w] = (is_negative == true) ? -color_value.color.color_value : color_value.color.color_value;
				}
			}
			tri_alpha += trans_speed;
			for(int q = 0; q < 3; ++q)
				rValue[q] = ((rand()%2) == 0) ? -trans_speed : trans_speed;

		}

	};

#endif

