/* libcmx generic video using SDL */

#include "cmx_video.h"
#include "cmx_error.h"
#include"SDL.h"
#include<cstdio>
#include<cstdlib>
#include<cassert>
#include<fstream>
#include<png.h>
#include<jpeglib.h>
#include<sstream>
#include<string.h>
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#ifdef __APPLE__
#include<sys/mman.h>
#include<unistd.h>
#include<fcntl.h>
#endif



namespace cmx {
	namespace video {
		Surface *newSurface(int w, int h) {
			Surface *temp = new Surface();
			temp->createSurface(w,h, 4);
			return temp;
		}
		void *s = 0;
		int offset = 0;

		void userReadData(png_structp pngPtr, png_bytep data, png_size_t length) {
			png_voidp a = png_get_io_ptr(pngPtr);
			memcpy((void*)data,((const char*)a)+offset, length);
			offset += length;
		}

		int Surface::loadImage(const void *buf, unsigned int size) {
			offset = 0;
			int x, y;
			int width, height;
			png_byte color_type;
			png_byte bit_depth;
			png_structp png_ptr;
	        png_infop info_ptr;
	        int number_of_passes;
			png_bytep * row_pointers;
			png_byte header[8];
			memcpy((char*)header, (const void*)buf, 8);
			unsigned char *buf_c = (unsigned char*)buf;
			buf_c += 8;

			if(png_sig_cmp(header, 0, 8) != 0) {
				std::cerr <<  "Not a valid png file\n";
				return -9;
			}
			png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			if (!png_ptr)
				   return -3;

			info_ptr = png_create_info_struct(png_ptr);

			if (!info_ptr)
					return -4;

			if(setjmp(png_jmpbuf(png_ptr))) {

				return -5;
			}
			png_set_read_fn(png_ptr,(void*)buf_c,userReadData);

			png_set_sig_bytes(png_ptr, 8);
			png_read_info(png_ptr, info_ptr);

			width = png_get_image_width(png_ptr, info_ptr);
			height = png_get_image_height(png_ptr, info_ptr);
			w = width;
			h = height;
			color_type = png_get_color_type(png_ptr, info_ptr);
			bit_depth = png_get_bit_depth(png_ptr, info_ptr);


			number_of_passes = png_set_interlace_handling(png_ptr);
			png_read_update_info(png_ptr, info_ptr);

			if (setjmp(png_jmpbuf(png_ptr)))
				return -6;

			assert(height > 0);
			row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
			assert(row_pointers != 0);
			for (y=0; y<height; y++) {
				row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr, info_ptr));
				assert(row_pointers[y] != 0);
			}
			png_read_image(png_ptr, row_pointers);
			w=width, h=height;
			int pixel_space = 3;
             if(color_type == PNG_COLOR_TYPE_RGB)
            	 pixel_space = 3;

	          if(color_type == PNG_COLOR_TYPE_RGBA)
	               pixel_space = 4;

            createSurface(w,h, 4);
	        Uint32 *buffer = (Uint32*)this->buffer;
			for(y=0;y<height;y++)  {
				png_byte *row = row_pointers[y];
				for(x=0; x<width;x++)
			    {
					png_byte *ptr = &(row[x*pixel_space]);
			        Uint32 *uptr = &buffer[x+y*width];
			        Uint32 value = _RGB(ptr[0], ptr[1], ptr[2]);
			        *uptr = value;
			      }
			}
			for (y=0; y<height; y++)
			     	 free(row_pointers[y]);
			free(row_pointers);
			png_destroy_info_struct (png_ptr, &info_ptr);
			png_destroy_read_struct (&png_ptr, 0, 0);
			return 1;
		}

		std::string lowerString(std::string s) {
			std::string temp;
			for(int i = 0; i < s.length(); ++i)
				temp += tolower(s[i]);

			return temp;
		}

		void Surface::saveImage(std::string image) {

			if(lowerString(image).find(".png") != -1) {
				png_byte color_type;
				png_byte bit_depth;
				png_structp png_ptr;
				png_infop info_ptr;

				png_bytep *row_pointers;
				FILE *fptr = fopen(image.c_str(), "wb");
				png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
				info_ptr = png_create_info_struct(png_ptr);
				if(!info_ptr) {
					std::cerr << "Error png.\n";
					return ;
				}
				if(setjmp(png_jmpbuf(png_ptr))) {
					std::cerr << "Error has occoured\n";
					return ;
				}

				png_init_io(png_ptr, fptr);

				png_set_IHDR(png_ptr, info_ptr, w, h,
						 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
						 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

				row_pointers = (png_bytep*)malloc (sizeof(png_bytep) * h);
				int i=0,z=0;
				assert(row_pointers != 0);
				for(i = 0; i < h; ++i) {
					row_pointers[i] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
					assert(row_pointers[i] != 0);
				}
				assert(this->buffer != 0 );
				png_write_info(png_ptr, info_ptr);
				unsigned int *buffer = (unsigned int*) this->buffer;
				for(i = 0; i < h; ++i) {
					//png_byte *row = row_pointers[i];
					unsigned int *row = (unsigned int*)row_pointers[i];
					for(z = 0; z < w; ++z)
					{
						row[z] = buffer[z+i*w];
					}
					png_write_row(png_ptr, (png_byte*)row);
				}
				png_write_end(png_ptr, NULL);

				png_destroy_info_struct (png_ptr, &info_ptr);
				png_destroy_write_struct(&png_ptr, 0);

				for(i = 0; i < h; ++i)
					free(row_pointers[i]);
				free(row_pointers);
			} else if(lowerString(image).find(".jpg") != -1) {

				jpeg_compress_struct comp_info;
				jpeg_error_mgr jerr;

				JSAMPROW row_ptr[1];

				FILE *fptr = fopen(image.c_str(), "w");
				if (!fptr)
					return;

				comp_info.err = jpeg_std_error(&jerr);
				jpeg_create_compress(&comp_info);
				jpeg_stdio_dest(&comp_info, fptr);

				comp_info.image_width = this->w;
				comp_info.image_height = this->h;
				comp_info.input_components = 3;
				comp_info.in_color_space = JCS_RGB;

				jpeg_set_defaults(&comp_info);
				jpeg_start_compress(&comp_info, TRUE);

		          unsigned char *buffer = (unsigned char*) calloc (1,  this->w * this->h * 3 );
		          unsigned int counter = 0;

		          unsigned char *ptr = buffer;
		          unsigned char *pix = (unsigned char*)this->buffer;

		          while(counter < (this->w*this->h)) {
		               unsigned int *ptr_val = (unsigned int *) pix;
		               unsigned char *arr = (unsigned char *)ptr_val;
		               ptr[0] = arr[0];
		               ptr[1] = arr[1];
		               ptr[2] = arr[2];
		               pix += 4;
		               ptr += 3;
		               counter++;
		          }

		          while( comp_info.next_scanline < comp_info.image_height ) {
		               row_ptr[0] = &buffer[ comp_info.next_scanline * comp_info.image_width * comp_info.input_components];
		               jpeg_write_scanlines (&comp_info, row_ptr, 1);
		          }
		          free(buffer);
		          jpeg_finish_compress(&comp_info);
		          jpeg_destroy_compress(&comp_info);
		          fclose(fptr);
			}
		}

		int Surface::loadImage(std::string image) {

			std::string lower = "";
			for (int i = 0; i < image.length(); ++i)
				lower += tolower(image[i]);
			if (lower.find(".png") != -1) {

				png_byte header[8];
				int x, y;
				int width, height;
				png_byte color_type;
				png_byte bit_depth;
				png_structp png_ptr;
				png_infop info_ptr;
				int number_of_passes;
				png_bytep * row_pointers;
				FILE *fp = fopen(image.c_str(), "rb");
				if (!fp)
					return -1;
				fread(header, 1, 8, fp);
				if (png_sig_cmp(header, 0, 8))
					return -2;
				png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
						NULL);
				if (!png_ptr)
					return -3;
				info_ptr = png_create_info_struct(png_ptr);
				if (!info_ptr)
					return -4;
				if (setjmp(png_jmpbuf(png_ptr)))
					return -5;

				png_init_io(png_ptr, fp);
				png_set_sig_bytes(png_ptr, 8);
				png_read_info(png_ptr, info_ptr);
				width = png_get_image_width(png_ptr, info_ptr);
				height = png_get_image_height(png_ptr, info_ptr);
				w = width;
				h = height;
				color_type = png_get_color_type(png_ptr, info_ptr);
				bit_depth = png_get_bit_depth(png_ptr, info_ptr);
				number_of_passes = png_set_interlace_handling(png_ptr);
				png_read_update_info(png_ptr, info_ptr);
				if (setjmp(png_jmpbuf(png_ptr)))
					return -6;
				assert(height > 0);
				row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
				assert(row_pointers != 0);
				for (y = 0; y < height; y++) {
					row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr, info_ptr));
					assert(row_pointers[y] != 0);
				}
				png_read_image(png_ptr, row_pointers);
				fclose(fp);
				w = width, h = height;
				int pixel_space = 3;
				if (color_type == PNG_COLOR_TYPE_RGB)
					pixel_space = 3;

				if (color_type == PNG_COLOR_TYPE_RGBA)
					pixel_space = 4;

				createSurface(w, h, 4);
				Uint32 *buffer = (Uint32*) this->buffer;
				for (y = 0; y < height; y++) {
					png_byte *row = row_pointers[y];
					for (x = 0; x < width; x++) {
						png_byte *ptr = &(row[x * pixel_space]);
						Uint32 *uptr = &buffer[x + y * width];
						Uint32 value = _RGB(ptr[0], ptr[1], ptr[2]);
						*uptr = value;
					}
				}
				for (y = 0; y < height; y++)
					free(row_pointers[y]);

				free(row_pointers);
				png_destroy_info_struct(png_ptr, &info_ptr);
				png_destroy_read_struct(&png_ptr, 0, 0);
				fclose(fp);
				return 1;
			}
			else if(lower.find(".jpg") != -1) {
				JSAMPROW row_ptr[1];
				FILE *fptr = fopen(image.c_str(), "rb");
				jpeg_decompress_struct deInfo;
				jpeg_error_mgr deErr;
				deInfo.err = jpeg_std_error(&deErr);
				jpeg_create_decompress(&deInfo);
				jpeg_stdio_src(&deInfo, fptr);
				jpeg_read_header(&deInfo, TRUE);
				jpeg_start_decompress(&deInfo);
				int width = deInfo.output_width, height = deInfo.output_height, depth = deInfo.num_components;
				int output_lines = deInfo.output_height;
				createSurface(width,height,4);
				row_ptr[0] = (unsigned char*) malloc(deInfo.output_width*deInfo.num_components);
				unsigned int i = 0;
				unsigned long location = 0;
				unsigned int *pix = (unsigned int*)this->buffer;
				unsigned char *cpix = (unsigned char*)this->buffer;
				int pos=0;
				while (deInfo.output_scanline < deInfo.image_height)
				{
					jpeg_read_scanlines(&deInfo, row_ptr, 1);
					for(i = 0; i <deInfo.image_width*deInfo.num_components; i += deInfo.num_components) {
						pix[pos++] = _RGB(row_ptr[0][i], row_ptr[0][i+1], row_ptr[0][i+2]);
					}
				}
				w = width, h = height, bpp = depth;
				bpp = 4;
				jpeg_finish_decompress(&deInfo);
				jpeg_destroy_decompress(&deInfo);
				fclose(fptr);
				free(row_ptr[0]);
				return 1;
			}

			return 0;

		}
	}
}

