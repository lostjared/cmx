#include<windows.h>
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#include"cmx_event.h"
#include<iostream>
#include<deque>
namespace cmx {
	namespace event {

		std::deque<Event > event_que;
        // fill in later when I have time
        unsigned int createTimer(unsigned int value, timerFunc t) {
        }
        void stopTimer(unsigned int id) {
            
        }

		bool Event::procEvent() {

			static MSG msg;
			int t = PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE);
			if(t) {
				if(!GetMessage(&msg, 0, 0, 0)) {
					type = EVENT_QUIT;
					return true;
				} else {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			if(!cmx::event::event_que.empty()) {
					*this = event_que.front();
					event_que.pop_front();
					return true;
			}

			return false;
		}

	}
}

