#include"cmx.h"
#include"cmx_video.h"
#include"cmx_joystick.h"

#include <cassert>
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
#include <psl1ght/lv2.h>

PadInfo padinfo;
PadData paddata;

namespace cmx {
	namespace input {
		void init_joystick() {
			ioPadInit(7);
		}
		bool poll_joystick(int num, int btn) {
			int i=num;
			ioPadGetInfo(&padinfo);
			if(padinfo.status[i]){
					ioPadGetData(num, &paddata);
					unsigned int btn_num[25] = { 0,
					paddata.BTN_LEFT,paddata.BTN_RIGHT, paddata.BTN_UP, paddata.BTN_DOWN,
					paddata.BTN_CROSS, paddata.BTN_CIRCLE,paddata.BTN_SQUARE,paddata.BTN_TRIANGLE,
					paddata.BTN_L1, paddata.BTN_L2, paddata.BTN_R1, paddata.BTN_R2, paddata.BTN_SELECT,
					paddata.BTN_START, 0 };
					if(btn >= 0 && btn <= BUTTON_START && btn_num[btn]) return true;
				}
			return false;
		}
		int num_joystick()  { return MAX_PADS; }
		void close_joystick() { }
	}
}
