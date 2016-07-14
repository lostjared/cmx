/* libcmx generic video using SDL */

#include "cmx_video.h"
#include "cmx_error.h"
#include<cstdio>
#include<cstdlib>
#include<cassert>
#include<fstream>
#include<png.h>
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

			w = info_ptr->width;
			h = info_ptr->height;
			width = info_ptr->width;
			height = info_ptr->height;
			color_type = info_ptr->color_type;
			bit_depth = info_ptr->bit_depth;


			number_of_passes = png_set_interlace_handling(png_ptr);
			png_read_update_info(png_ptr, info_ptr);

			if (setjmp(png_jmpbuf(png_ptr)))
				return -6;

			assert(height > 0);
			row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
			assert(row_pointers != 0);
			for (y=0; y<height; y++) {
				row_pointers[y] = (png_byte*) malloc(info_ptr->rowbytes);
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
	        unsigned int *buffer = (unsigned int*)this->buffer;
			for(y=0;y<height;y++)  {
				png_byte *row = row_pointers[y];
				for(x=0; x<width;x++)
			    {
					png_byte *ptr = &(row[x*pixel_space]);
			        unsigned int *uptr = &buffer[x+y*width];
			        unsigned int value = _RGB(ptr[0], ptr[1], ptr[2]);
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

		void Surface::saveImage(std::string image) {

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
				row_pointers[i] = (png_byte*)malloc(info_ptr->rowbytes);
				assert(row_pointers[i] != 0);
			}
			assert( this->buffer != 0 );
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
			for(i = 0; i < h; ++i)
				free(row_pointers[i]);
			free(row_pointers);
			png_destroy_info_struct (png_ptr, &info_ptr);
			png_destroy_write_struct(&png_ptr, 0);
		}

		int Surface::loadImage(std::string image) {

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
			png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
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
			w = info_ptr->width;
			h = info_ptr->height;
			width = info_ptr->width;
			height = info_ptr->height;
			color_type = info_ptr->color_type;
			bit_depth = info_ptr->bit_depth;

			number_of_passes = png_set_interlace_handling(png_ptr);
			png_read_update_info(png_ptr, info_ptr);
			if (setjmp(png_jmpbuf(png_ptr)))
				return -6;
			assert(height > 0);
			row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
			assert(row_pointers != 0);
			for (y=0; y<height; y++) {
				row_pointers[y] = (png_byte*) malloc(info_ptr->rowbytes);
				assert(row_pointers[y] != 0);
			}
			png_read_image(png_ptr, row_pointers);
			fclose(fp);
			w=width, h=height;
			int pixel_space = 3;
             if(color_type == PNG_COLOR_TYPE_RGB)
            	 pixel_space = 3;

	          if(color_type == PNG_COLOR_TYPE_RGBA)
	               pixel_space = 4;

            createSurface(w,h, 4);
	        unsigned int *buffer = (unsigned int*)this->buffer;
			for(y=0;y<height;y++)  {
				png_byte *row = row_pointers[y];
				for(x=0; x<width;x++)
			    {
					png_byte *ptr = &(row[x*pixel_space]);
			        unsigned int *uptr = &buffer[x+y*width];
			        unsigned int value = _RGB(ptr[0], ptr[1], ptr[2]);
			        *uptr = value;
			      }
			}
			for (y=0; y<height; y++)
			     	 free(row_pointers[y]);
			free(row_pointers);
			png_destroy_info_struct (png_ptr, &info_ptr);
			png_destroy_read_struct (&png_ptr, 0, 0);
			fclose(fp);
			return 1;
		}
	}
}

