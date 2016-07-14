#ifndef CMX_EVENT_H_
#define CMX_EVENT_H_
#include "cmx_joystick.h"
#include<deque>

namespace cmx {
	namespace event {
		enum EventTypes { EVENT_NOTHING, EVENT_KEYUP, EVENT_KEYDOWN, EVENT_MOUSE, EVENT_QUIT };
		enum { K_NULL=0, K_LEFT, K_RIGHT, K_UP, K_DOWN, K_RETURN, K_SPACE, K_ESCAPE};
		class Event {
		public:
			EventTypes type;
			bool translate;
			int skey, key, x, y, button, state;
			bool procEvent();

			Event &operator=( const Event &e ) {
				type = e.type;
				translate = e.translate;
				skey = e.skey;
				key = e.key;
				x = e.x;
				y = e.y;
				button = e.button;
				state = e.state;
				return *this;
			}

			bool translate_joy() {
				static int btn[12][12];
				bool rt_value = false;
				for(int i = 0; i < cmx::input::num_joystick(); ++i) {
					for(int z = 0; z <= cmx::input::BUTTON_START; z++) {
						if(cmx::input::poll_joystick(i, z)) {
							type = EVENT_KEYDOWN;
							if(btn[i][z] != 1) {
								btn[i][z] = 1;
								switch(z) {
								case cmx::input::BUTTON_UP:
									skey = cmx::event::K_UP;
									break;
								case cmx::input::BUTTON_DOWN:
									skey = cmx::event::K_DOWN;
									break;
								case cmx::input::BUTTON_LEFT:
									skey = cmx::event::K_LEFT;
									break;
								case cmx::input::BUTTON_RIGHT:
									skey = cmx::event::K_RIGHT;
									break;
								case cmx::input::BUTTON_START:
									skey = cmx::event::K_RETURN;
									break;
									}
									rt_value = true;
								}
							} else btn[i][z] = 0;
						}
				}
				return rt_value;
			}
		};

		extern std::deque<Event > event_que;
        typedef unsigned int (*timerFunc)(unsigned int, void *);
        unsigned int createTimer(unsigned int interval, timerFunc t);
        void stopTimer(unsigned int value);
        
	}
}

#endif




