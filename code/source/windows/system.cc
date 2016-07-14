#define WIN32_LEAN_AND_MEAN
#include<windows.h>
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#include<cstring>
#include<cassert>
#include "cmx_system.h"
#include "cmx_video.h"
#include "cmx_event.h"
#include "cmx_gl.h"
#include<GL/gl.h>
#include<GL/glu.h>

void setupPixelFormat(HDC);
HWND hwnd;

static HDC hDC;


// lame way
int transkey(int key) {
	switch(key) {
	case VK_LEFT:
		return cmx::event::K_LEFT;
		break;
	case VK_RIGHT:
		return cmx::event::K_RIGHT;
		break;
	case VK_UP:
		return cmx::event::K_UP;
		break;
	case VK_DOWN:
		return cmx::event::K_DOWN;
		break;
	case VK_ESCAPE:
		return cmx::event::K_ESCAPE;
		break;
	case VK_SPACE:
		return cmx::event::K_SPACE;
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	static HGLRC hRC;

	switch(msg) {
	case WM_DESTROY: {
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);
		PostQuitMessage(0);
		cmx::event::Event e;
		e.type = cmx::event::EVENT_QUIT;
		cmx::event::event_que.push_back(e);
	}
		break;
	case WM_ACTIVATEAPP:
		cmx::event::active = (bool) wParam;
		break;
	case WM_CREATE: {
		hDC = GetDC(hwnd);
		setupPixelFormat(hDC);
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
	}
		break;
	case WM_SIZE: {

		UINT width, height;
		width = LOWORD(lParam);
		height= HIWORD(lParam);
		if(height == 0)
			height = 1;
		glViewport(0,0,width,height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, width, 0, height);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	break;
	case WM_KEYDOWN:{
		int key = wParam;
		cmx::event::Event e;
		e.type = cmx::event::EVENT_KEYDOWN;
		if(key >= 'A' && key <= 'Z') e.key = tolower(key);
		else e.key = key;
		e.skey = transkey(key);
		cmx::event::event_que.push_back(e);
	}
	break;
	case WM_KEYUP:{
		int key = wParam;
		cmx::event::Event e;
		e.type = cmx::event::EVENT_KEYUP;
		if(key >= 'A' && key <= 'Z') e.key = tolower(key);
		else e.key = key;
		e.skey = transkey(key);
		cmx::event::event_que.push_back(e);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		cmx::event::Event e;
		e.type = cmx::event::EVENT_MOUSE;
		e.button = 1;
		e.state = 1;
		e.x = LOWORD(lParam);
		e.y = HIWORD(lParam);
		cmx::event::event_que.push_back(e);
	}
	break;
	case WM_LBUTTONUP: {
		cmx::event::Event e;
		e.type = cmx::event::EVENT_MOUSE;
		e.button = 1;
		e.state = 0;
		e.x = LOWORD(lParam);
		e.y = HIWORD(lParam);
		cmx::event::event_que.push_back(e);
	}
		break;
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN: {
		cmx::event::Event e;
		e.type = cmx::event::EVENT_MOUSE;
		e.button = 1;
		e.state = (msg == WM_RBUTTONUP) ? 0 : 1;
		e.x = LOWORD(lParam);
		e.y = HIWORD(lParam);
		cmx::event::event_que.push_back(e);
	}
		break;
	default:
		return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}

void setupPixelFormat(HDC hDC) {
	int nPixelFormat;
	int bits = GetDeviceCaps(GetDC(NULL),BITSPIXEL);

	static	PIXELFORMATDESCRIPTOR pfd=					{
		sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			bits,
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			bits,
			0,
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
	};
	nPixelFormat = ChoosePixelFormat(hDC,&pfd);
	SetPixelFormat(hDC,nPixelFormat,&pfd);
}

void initClass() {
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hIcon = wcex.hIconSm = (HICON)LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = "cmx";
	wcex.lpszMenuName = 0;
	wcex.lpfnWndProc = (WNDPROC) WindowProc;
	wcex.hInstance = GetModuleHandle(0);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wcex);
}

void initWindow(int w, int h, bool full) {

	hwnd = CreateWindow("cmx", "[cmx]", (full == false) ? WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX : WS_POPUPWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,w ,h, 0, 0, GetModuleHandle(0), 0);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	cmx::input::extra = (void*)&hwnd;
}

namespace cmx {
	namespace system {

		int width,height;
		cmx::video::Surface *back = 0;

		int init_system() {
			initClass();
			return 1;
		}

		int stop_system() {
			if(back != 0) delete back;
			PostQuitMessage(0);
			return 0;
		}

		unsigned int tick_system() {
			return GetTickCount();
		}

		void toggle_fullscreen() {
			DEVMODE dev;
			memset(&dev,0,sizeof(dev));
			dev.dmSize = sizeof(dev);
			dev.dmPelsWidth = width;
			dev.dmPelsHeight = height;
			dev.dmBitsPerPel = GetDeviceCaps(GetDC(NULL),BITSPIXEL);
			dev.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
			if(ChangeDisplaySettings(&dev,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				MessageBox(NULL,"Error couldnt change display mode","Error",MB_ICONERROR);
			}
		}

		video::Surface *init_surface(int SCREEN_WIDTH, int SCREEN_HEIGHT) {
			width = SCREEN_WIDTH;
			height = SCREEN_HEIGHT;
			initWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false);
			glViewport(0, 0, width, height);
			glClearColor(0, 0, 0, 0);
			glClearDepth(1.0f);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(0, width, 0, height);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			back = cmx::video::newSurface(SCREEN_WIDTH, SCREEN_HEIGHT);
			return back;
		}

		bool copy_surface(video::Surface &surface) {
			glPixelZoom(1, -1);
			glRasterPos2f(0, height);
			glDrawPixels(surface.w, surface.h, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)surface.buffer);
			SwapBuffers(hDC);
			return true;
		}

		void *direct_system_memory() {
			return (void*)back->buffer;
		}

		void swap_memory() {
			if(back != 0) copy_surface(*back);
		}

		void check_callbacks() { }
	}

	namespace gl {
		void swap_gl_buffers() {
			SwapBuffers(hDC);
		}
	}
}

