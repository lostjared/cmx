
#include"cmx_font.h"
#include<cstdio>
#include<cstdlib>
#include<cstring>
namespace cmx {

	namespace font {

		int readInt(void *buffer) {
			int temp=0;
			memcpy(&temp, buffer, sizeof(int));
			return temp;
		}

		bool Font::loadFont(const void *buffer, unsigned int size) {

			int *font_buffer = (int*)buffer;
			_Font *fnt = &font_source;

			int temp=0;
			temp=readInt(font_buffer);
			font_buffer++;
			if(temp == -100) {
				int cx = readInt(font_buffer);


				font_buffer++;
				int cy = readInt(font_buffer);
				font_buffer++;
				int tcolor = readInt(font_buffer);
				fnt->tcolor = tcolor;
				font_buffer++;

			if(cx < 0 || cy < 0 || cx > 250 || cy > 250) {
				fprintf(stderr, "Error invalid font file format!\n");
				return false;
			  }
			initFont(cx, cy,tcolor);
			int p=0,i=0,z=0;
			for(p = 0; p <= 127; p++)
			for(i = 0; i < fnt->mx; i++) {
				for(z = 0; z < fnt->my; z++) {
					   temp = readInt(font_buffer);
					   font_buffer++;
					   fnt->letters[p].fnt_ptr[i][z] = temp;
				   }
			  }
			} else {
				fprintf(stderr, "Error font corrupted..\n");
				return false;
			}
			return true;
		}

		bool Font::loadFont(const char *src) {

		 FILE *fptr = fopen(src, "rb");
		 int q = 0,mx = 0,my = 0;
		 int i,z,p;
		 if(!fptr) return false;
		 int mode = 0;
		 _Font *fnt =&this->font_source;
		  mode = 0;
		 fread((char*)&q, sizeof(int), 1, fptr);
		 if(mode == 1)
		 q = littleToBig(q);
		 if(q == -100) {
			  fread((char*)&mx, sizeof(int), 1, fptr);
			  fread((char*)&my, sizeof(int), 1, fptr);
			  fread((char*)&fnt->tcolor, sizeof(int), 1, fptr);
			  if(mode == 1) {
				   mx = littleToBig(mx);
				   my = littleToBig(my);
				   fnt->tcolor = littleToBig(fnt->tcolor);
			  }
			  if(mx < 0 || mx < 0 || mx > 250 || my > 250) {
				   // invalid font file
				fprintf(stderr, "Error invalid font file format!\n");
				 return false;
			  }
			  initFont(mx, my, fnt->tcolor);
			  for(p = 0; p <= 127; p++)
			  for(i = 0; i < fnt->mx; i++) {
				   for(z = 0; z < fnt->my; z++) {
						fread((char*)&fnt->letters[p].fnt_ptr[i][z], sizeof(int), 1, fptr);
						if(mode == 1) fnt->letters[p].fnt_ptr[i][z] = littleToBig(fnt->letters[p].fnt_ptr[i][z]);
				   }
			  }
		 }
		 return true;
		}
	}
}

