
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#include"cmx_event.h"
#include"SDL.h"
#include<iostream>
#include<vector>

namespace cmx {
	namespace event {


		std::vector<_SDL_TimerID *> timers;

        unsigned int createTimer(unsigned int value, timerFunc t) {
            _SDL_TimerID *ti = SDL_AddTimer(value, t, 0);
            if(ti == NULL) {
            	std::cerr << "Error could not create timer\n";
            	exit(0);
            }
            timers.push_back(ti);
            unsigned int index = timers.size()-1;
            std::cout << "Created Timer with Index: " << index << "\n";
            return index;
        }
        
        void stopTimer(unsigned int id) {
            SDL_RemoveTimer(timers[id]);
        }

		std::deque<Event > event_que;

		int transKey(int k) {
			switch(k) {
			case SDLK_LEFT:
				return K_LEFT;
			case SDLK_RIGHT:
				return K_RIGHT;
			case SDLK_UP:
				return K_UP;
			case SDLK_DOWN:
				return K_DOWN;
			case SDLK_RETURN:
				return K_RETURN;
			case SDLK_SPACE:
				return K_SPACE;
			case SDLK_ESCAPE:
				return K_ESCAPE;
			default:
				break;
			}
			return k;
		}

		bool Event::procEvent() {
			static SDL_Event e;

			type = EVENT_NOTHING;
			key = 0;
			skey = 0;
			button = 0;
			state = 0;

			if(SDL_PollEvent(&e)) {
				switch(e.type) {
				case SDL_KEYUP:
				case SDL_KEYDOWN:
					type = (e.type == SDL_KEYUP) ? EVENT_KEYUP : EVENT_KEYDOWN;
					key = e.key.keysym.sym;
					skey = transKey(e.key.keysym.sym);
					break;
				case SDL_QUIT:
					type = EVENT_QUIT;
					state = EVENT_QUIT;
					break;
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN:
					type = EVENT_MOUSE;
					x = e.button.x;
					y = e.button.y;
					button = e.button.button;
					state = (e.type == SDL_MOUSEBUTTONUP) ? 1 : 2;
					break;
				}
				return true;
			} else return false;

			if(translate == true) {
				return translate_joy();
			}
			return false;
		}

	}
}

