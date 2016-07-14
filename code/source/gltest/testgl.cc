#include<cmx.h>
#include"SDL.h"
#include<cmx_gl.h>
#ifdef _WIN32
#include<windows.h>
#include<GL/gl.h>
#include<GL/glu.h>
#else
#ifdef __APPLE__
#include<OpenGL/gl.h>
#include<OpenGL/glu.h>
#else
#include<GL/gl.h>
#include<GL/glu.h>
#endif
#endif


#include<cassert>

// function prototypes
void setupGL();
void renderGL();
// main function

extern "C" int main(int argc, char **argv) {
	assert(cmx::system::init_system() == 1); // init the system
	cmx::video::Surface *buffer = cmx::gl::init_gl_buffer(); // init the opengl buffer
	cmx::input::init_joystick(); // init the joystick

	setupGL();// setup gl
	bool active = true;
	while (active == true) {
		static cmx::event::Event e;
		while (e.procEvent()) {
			switch (e.type) {
			case cmx::event::EVENT_QUIT:
				active = false;
				break;
			default:
				break;
			}
		}
		renderGL();// render Scene
		cmx::gl::swap_gl_buffers();// swap buffers
	}
	cmx::input::close_joystick();
	return cmx::system::stop_system();
}

void setupGL() {
	glViewport(0, 0, cmx::system::width, cmx::system::height);
	glClearDepth(1.0f);
	glClearColor(0, 0, 0, 0);
	glMatrixMode( GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 2, 0, 2);
	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity();
}

void renderGL() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(0.9f, 0.5f, 0);
	glBegin( GL_TRIANGLES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 1, 0);
	glColor3f(0, 1, 0);
	glVertex3f(1, -1, 0);
	glColor3f(0, 0, 1);
	glVertex3f(-1, -1, 0);
	glEnd();
	glPopMatrix();
}
