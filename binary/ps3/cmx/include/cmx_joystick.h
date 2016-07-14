#ifndef __CMX__JOYSTICK__H_
#define __CMX__JOYSTICK__H_

namespace cmx {
	namespace input {

		enum { BUTTON_LEFT=0, BUTTON_RIGHT, BUTTON_UP, BUTTON_DOWN, BUTTON_CROSS, BUTTON_CIRCLE, BUTTON_SQUARE, BUTTON_TRIANGLE, BUTTON_START };

		void init_joystick();
		bool poll_joystick(int num, int btn);
		int num_joystick();
		void close_joystick();
	}
}


#endif


