#include "cmx_gl.h"
#include "SDL.h"
#include "SDL_opengl.h"
#ifdef _WIN32
#include<GL/gl.h>
#include<GL/glu.h>
#endif

#ifdef __APPLE__
#include<OpenGL/gl.h>
#include<OpenGL/glu.h>
#endif



namespace cmx {

	namespace gl {

		SDL_Surface *front;
		SDL_Surface *copy_surf;
		bool fullscreen_=false;

		cmx::video::Surface *init_gl_buffer(int w, int h) {
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
            SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
            SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
			front = SDL_SetVideoMode(w, h, 32, SDL_OPENGL);
			system::width = w;
			system::height = h;
			return cmx::video::newSurface(w, h);
		}

		void toggle_fullscreen() {
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
			unsigned int flags = 0;
			static bool var = false;
			var = !var;
			if(var) flags = SDL_OPENGL | SDL_FULLSCREEN;
			else flags = SDL_OPENGL;
			front = SDL_SetVideoMode(cmx::system::width, cmx::system::height, 32,flags);
			fullscreen_ = !fullscreen_;
			system::width = front->w;
			system::height = front->h;
		}

		void swap_gl_buffers() {
			SDL_GL_SwapBuffers();
		}

		void draw_surface(cmx::video::Surface *surface) {
			glViewport(0, 0, front->w, front->h);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(0, surface->w, 0, surface->h);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			if(fullscreen_) {
#ifndef _WIN32
				//if(surface->h == 720) glWindowPos2i(0, 760);
				//if(surface->h == 1080) glWindowPos2i(0,1140);
#else
				glRasterPos2i(0, surface->h);
#endif
			}
			else
				glRasterPos2i(0, surface->h);

			glPixelZoom(1, -1);
			glDrawPixels(surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)surface->buffer);
		}

	}

}
