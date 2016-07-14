#include "SDL.h"
#include"cmx_joystick.h"

#include<iostream>

namespace cmx {

	namespace input {

		void *extra;

		SDL_Joystick *stick[4];
		int num_joy=SDL_NumJoysticks();
		int joy_offset = 0;

		void close_joystick() {
			for(int i = 0; i < joy_offset; i++) {
				if(stick[i]) SDL_JoystickClose(stick[i]);
			}
		}

		void init_joystick() {
			for(int i = 0; i  < SDL_NumJoysticks(); i++) {
				SDL_Joystick *st = SDL_JoystickOpen(i);
				if(st != 0) {
					stick[joy_offset] = st;
					std::cout << "Joystick: " << SDL_JoystickName(i) << " at offet: " << joy_offset << "\n";
					joy_offset++;
				}
			}
		}

		int num_joystick() { return joy_offset; }

		bool poll_joystick(int num, int btn) {
			unsigned int btn_arr[12] = {SDL_HAT_LEFT, SDL_HAT_RIGHT,SDL_HAT_UP,SDL_HAT_DOWN,1,3,4,2,8,0};
			if(btn >= BUTTON_LEFT && btn <= BUTTON_DOWN) {
				int hat = SDL_JoystickGetHat(stick[num], 0);
				if(hat & btn_arr[btn]) return true;
			}
			if(btn > BUTTON_DOWN && btn <= 12 && SDL_JoystickGetButton(stick[num], btn_arr[btn])) return true;
			return false;
		}
	}
}

