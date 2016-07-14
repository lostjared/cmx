#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

#include"cmx_system.h"
#include"cmx_video.h"
#include<cassert>
#include <psl1ght/lv2.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sysutil/video.h>
#include <rsx/gcm.h>
#include <rsx/reality.h>
#include <io/pad.h>
#include <sysmodule/sysmodule.h>
#include <pngdec/pngdec.h>
#include "sysutil/events.h"

gcmContextData *context; // Context to keep track of the RSX buffer.

VideoResolution res; // Screen Resolution
int &width = cmx::system::width, &height = cmx::system::height;
int currentBuffer = 0;
s32 *buffer[2]; // The buffer we will be drawing into.

void waitFlip() { // Block the PPU thread untill the previous flip operation has finished.
	while(gcmGetFlipStatus() != 0)
		usleep(200);
	gcmResetFlipStatus();
}

void flip(s32 buffer) {
	assert(gcmSetFlip(context, buffer) == 0);
	realityFlushBuffer(context);
	gcmSetWaitFlip(context); // Prevent the RSX from continuing until the flip has finished.
}

// Initilize everything. You can probally skip over this function.
void init_screen() {
	// Allocate a 1Mb buffer, alligned to a 1Mb boundary to be our shared IO memory with the RSX.
	void *host_addr = memalign(1024*1024, 1024*1024);
	assert(host_addr != NULL);

	// Initilise Reality, which sets up the command buffer and shared IO memory
	context = realityInit(0x10000, 1024*1024, host_addr);
	assert(context != NULL);

	VideoState state;
	assert(videoGetState(0, 0, &state) == 0); // Get the state of the display
	assert(state.state == 0); // Make sure display is enabled

	// Get the current resolution
	assert(videoGetResolution(state.displayMode.resolution, &res) == 0);

	// Configure the buffer format to xRGB
	VideoConfiguration vconfig;
	memset(&vconfig, 0, sizeof(VideoConfiguration));
	vconfig.resolution = state.displayMode.resolution;
	vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
	vconfig.pitch = res.width * 4;
	vconfig.aspect=state.displayMode.aspect;

	assert(videoConfigure(0, &vconfig, NULL, 0) == 0);
	assert(videoGetState(0, 0, &state) == 0);

	s32 buffer_size = 4 * res.width * res.height; // each pixel is 4 bytes
	printf("buffers will be 0x%x bytes\n", buffer_size);

	width=res.width;
	height=res.height;
	gcmSetFlipMode(GCM_FLIP_VSYNC); // Wait for VSYNC to flip

	// Allocate two buffers for the RSX to draw to the screen (double buffering)
	buffer[0] = (s32*) rsxMemAlign(16, buffer_size);
	buffer[1] = (s32*) rsxMemAlign(16, buffer_size);
	assert(buffer[0] != NULL && buffer[1] != NULL);

	u32 offset[2];
	assert(realityAddressToOffset(buffer[0], &offset[0]) == 0);
	assert(realityAddressToOffset(buffer[1], &offset[1]) == 0);
	// Setup the display buffers
	assert(gcmSetDisplayBuffer(0, offset[0], res.width * 4, res.width, res.height) == 0);
	assert(gcmSetDisplayBuffer(1, offset[1], res.width * 4, res.width, res.height) == 0);

	gcmResetFlipStatus();
	flip(1);
}

static void eventHandle(u64 status, u64 param, void * userdata) {
    (void)param;
    (void)userdata;
	if(status == EVENT_REQUEST_EXITAPP){
		printf("Quit game requested\n");
		exit(0);
	}else{
		printf("Unhandled event: %08llX\n", (unsigned long long int)status);
	}
}

u32 module_flag;

void unload_modules(){

	if(module_flag & 2)
		SysUnloadModule(SYSMODULE_PNGDEC);

	if(module_flag & 1)
		SysUnloadModule(SYSMODULE_FS);
}

namespace cmx {
	namespace system {

		int width,height;

	   int init_system() {
		   if(SysLoadModule(SYSMODULE_FS)!=0) return 0; else module_flag |=1;
		   if(SysLoadModule(SYSMODULE_PNGDEC)!=0) return 0; else module_flag |=2;

		   sysRegisterCallback(EVENT_SLOT0, eventHandle, NULL);

		   init_screen();
		   return 1;
		}
		int stop_system() {
			unload_modules();
			sysUnregisterCallback(EVENT_SLOT0);
			exit(0);
			return 0;
		}
		unsigned int tick_system() {
			return 0;
		}
		video::Surface *init_surface(int , int) {
			video::Surface *s = new video::Surface();
			s->createSurface(width, height, 4);
			return s;
		}

		void toggle_fullscreen() { }

		bool copy_surface(video::Surface &surface) {
			waitFlip(); // Wait for the last flip to finish, so we can draw to the old buffer
			unsigned int *sbuf = (unsigned int *)surface.buffer;
			unsigned int *buf = (unsigned int *)buffer[currentBuffer];
			for(int i = 0; i < width; i++) {
				for(int z = 0; z < height; z++) {
					buf[i+z*width] = sbuf[i+z*surface.w];
				}
			}
			//memcpy(buffer[currentBuffer], surface.buffer, surface.w*surface.h*surface.bpp-1);
			flip(currentBuffer); // Flip buffer onto screen
			currentBuffer = !currentBuffer;
			sysCheckCallback();
			return true;
		}

		void check_callbacks() {
			sysCheckCallback();
		}

		void *direct_system_memory() { waitFlip(); return buffer[currentBuffer]; }
		void swap_memory() {
			flip(currentBuffer);
			currentBuffer = !currentBuffer;
			sysCheckCallback();
		}
	}
}
