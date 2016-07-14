#ifndef __CMX__JOYSTICK__H_
#define __CMX__JOYSTICK__H_

namespace cmx {
	namespace input {

		extern void *extra;

		enum { BUTTON_LEFT=1, BUTTON_RIGHT, BUTTON_UP, BUTTON_DOWN, BUTTON_CROSS, BUTTON_CIRCLE, BUTTON_SQUARE, BUTTON_TRIANGLE, BUTTON_L1, BUTTON_L2, BUTTON_R1, BUTTON_R2, BUTTON_SELECT, BUTTON_START };

		extern void init_joystick();
		bool poll_joystick(int num, int btn);
		int num_joystick();
		void close_joystick();
	}
}


#endif


