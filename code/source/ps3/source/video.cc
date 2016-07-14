/* libcmx generic video using SDL */
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#include "cmx_video.h"
#include "cmx_error.h"
#include<cstdio>
#include<cstdlib>
#include<cassert>
#include<fstream>
#include<pngdec/loadpng.h>

namespace cmx {
	namespace video {
		Surface *newSurface(int w, int h) {
			Surface *temp = new Surface();
			temp->createSurface(w,h, 4);
			return temp;
		}

		int Surface::loadImage(const void *buffer, unsigned int bsize) {
			PngDatas png1;
			png1.png_in = (void*) buffer;
			png1.png_size = bsize;
			LoadPNG(&png1, NULL);
			w = png1.width;
			h = png1.height;
			this->buffer = (void*)png1.bmp_out;
			bpp = 4;
			pitch = png1.wpitch;
			release_on = 0;
			return 1;
		}


		int Surface::loadImage(std::string image) {
			PngDatas data = { 0 };
			LoadPNG(&data, image.c_str());
			w = data.width;
			h = data.height;
			pitch = data.wpitch;
			if(data.bmp_out == 0) return 0;
			this->buffer = (void*)data.bmp_out;
			release_on = 0;
			bpp = 4;
			return 1;
		}

	}

}
