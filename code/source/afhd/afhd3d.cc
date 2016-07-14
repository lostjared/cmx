#include<cmx.h>
#include<cmx_gl.h>
#include<iostream>
#include<cassert>
#include<sstream>
#ifdef __APPLE__
#include<OpenGL/gl.h>
#include<OpenGL/glu.h>
#else
#include<GL/gl.h>
#include<GL/glu.h>
#endif

#include"afhd.h"
#include"fixedsys.bin.h"

float depth = 0.0f;
int direction_ = 0;

GLfloat vertices[] = {
	-1.0f, -1.0f, 1.0f, /* front face */
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,

	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,

	-1.0f, -1.0f, -1.0f, /* left side */
	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f, /* top */
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,

	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f, /* bottom */
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,

	1.0f, -1.0f, -1.0f, /* right */
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,

	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,

	-1.0f, -1.0f, -1.0f, /* back face */
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,

};

GLfloat texCoords[] = {
                    0, 0, /* front */
                    1, 1,
                    0, 1,

                    0, 0,
                    1, 0,
                    1, 1,

                    0, 0, /* left */
                    1, 0,
                    0, 1,

                    0, 1,
                    1, 0,
                    1, 1,

                    0,0, /* top */
                    0,1,
                    1,1,

                    1, 1,
                    1, 0,
                    0, 0,

                    0, 0,/* bottom */
                    0, 1,
                    1, 1,

                    1,1,
                    1,0,
                    0,0,

                    0,0,/* right */
                    1,0,
                    0,1,

                    0,1,
                    1,0,
                    1,1,

                    0,0, /* back */
                    1,1,
                    0,1,

                    0,0,
                    1,0,
                    1,1
    };

GLuint texID;

cmx::font::Font the_font;

void reloadTexture(cmx::video::Surface *surface) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->buffer);
}

float speed = 0.1f;
float rot_speed =  6.0f;
bool direction = true;


GLfloat frontFace[] = {
          -1.0f, -1.0f, 1.0f, /* front face */
          1.0f, 1.0f, 1.0f,
          -1.0f, 1.0f, 1.0f,
          -1.0f, -1.0f, 1.0f,
          1.0f, -1.0f, 1.0f,
          1.0f, 1.0f, 1.0f
};

GLfloat frontTexture[] = {
          0, 0, /* front */
          1, 1,
          0, 1,

          0, 0,
          1, 0,
          1, 1,
};


void drawT(af &_x, cmx::video::Surface *surface) {

	static float rot_y = 0.0f, rot_ = 0.0f;
	reloadTexture(surface);

	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);
	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity();
	/* draw in 2D */
	glBindTexture(GL_TEXTURE_2D, texID);
	glDisable(GL_DEPTH_TEST);
	glEnableClientState( GL_VERTEX_ARRAY);
	glEnableClientState( GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, frontFace);
	glTexCoordPointer(2, GL_FLOAT, 0, frontTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, cmx::system::width/cmx::system::height, 0.1f, 300.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 120, 0, 0, 0, 0, 1, 0);
	glTranslatef(0, 0, -10);
	glTranslatef(0, 0, depth);
	switch(direction_) {
	case 0:
		glRotatef(rot_y, 0, 0 , 1);
		break;
	case 1:
		glRotatef(rot_y, 1, 0, 0);
		glRotatef(rot_y, 0, 1, 0);
		break;
	case 2:
		glRotatef(rot_y, 0, 1, 0);
		glRotatef(rot_y, 1, 0, 0);
		break;
	}
	rot_ += rot_speed;

	if(direction == true) {
		rot_y += rot_speed;
		depth += (depth > 120) ? 0.001f : speed;
	}
	else {
		depth -= speed;
		rot_y -= rot_speed;
	}

	if(depth > 129)
	{
		depth = 129;
		direction = false;
	} else if(depth <= 0) {
		direction = true;
		depth = 0;
	}

	glBindTexture(GL_TEXTURE_2D, texID);
	glEnableClientState( GL_VERTEX_ARRAY);
	glEnableClientState( GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();
}

int max_items = 6;

/* Use af to manipulate and copy to surface */
void calculatePixels(int frequency, af *_x, cmx::video::Surface *surface) {
	switch(frequency) {
	case 0: {
			static unsigned int o = 0;
			_x->TriangleShift(_x->image_files[o++].the_surface,surface);
			_x->Stoned(surface);
			if(o > 3)
				o = 0;
	}
		break;
	case 1:
		_x->SquareBlend(surface);
		break;
	case 2:
		_x->Stoned(surface);
		break;
	case 3:
		_x->TriBlendParadox(surface);
		break;
	case 4:
		_x->SquareBlend(surface);
		_x->Label(surface);
		break;
	case 5:
		_x->ThoughtThis(surface);
		_x->Label(surface);
		_x->SquareBlend(surface);
		break;
	case 6:
		_x->ShiftPixels(surface);
		break;
	}
}

void loadTextures(cmx::video::Surface *surface, af *_x) {
	glGenTextures(1, &texID);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->buffer);
}

int main(int argc, char **argv) {
	if(argc != 2) {
		std::cerr << "Error: argument requires list file.\n";
		exit(-1);
	}
	af _x;
	_x.buildImages(argv[1], 320, 240);
	cmx::video::Surface *back_surface, *temp_surface;
	the_font.loadFont((const void*)fixedsys_bin, sizeof(fixedsys_bin));
	assert(cmx::system::init_system() == 1);
	back_surface = cmx::gl::init_gl_buffer(1280, 720);
	temp_surface = cmx::video::newSurface(320, 240);
/*	cmx::gl::toggle_fullscreen(); */
	_x.buildStartImage(temp_surface);
	loadTextures(temp_surface, &_x);
	glViewport(0, 0, back_surface->w,back_surface->h);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, cmx::system::width/cmx::system::height, 0.1f, 300.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	int frequency=0;
	bool active=true;
	try {
		while (active) {
			static cmx::event::Event e;
			while (e.procEvent()) {
				switch (e.type) {
				case cmx::event::EVENT_QUIT:
					active = false;
					break;
				case cmx::event::EVENT_KEYUP:
					if (e.skey == cmx::event::K_ESCAPE)
						active = false;
					else if (e.key == 'a')
						std::cout << depth << "\n";
					else if (e.skey == cmx::event::K_UP) {
						if (frequency < max_items)
							frequency++;
					} else if (e.skey == cmx::event::K_DOWN) {
						if (frequency > 0)
							frequency--;
					} else if(e.skey == cmx::event::K_LEFT) {
							if(direction_ > 0) direction_ --;
					}
					else if(e.skey == cmx::event::K_RIGHT) {
						   if(direction_ < 2) direction_++;
					}else if(e.skey == cmx::event::K_SPACE) {
							depth += 20;
							direction = true;
					} else if(e.key == '4')
					{
						_x.resetAlpha();

					}
					break;
				default:
					break;
				}

			}
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			calculatePixels(frequency, &_x, temp_surface);
			drawT(_x, temp_surface);
			cmx::gl::swap_gl_buffers();
		}

	}
	catch(...) {
		delete temp_surface;
		return cmx::system::stop_system();
	}
	delete temp_surface;
	return cmx::system::stop_system();
}
