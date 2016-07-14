/* cmx - dinput - jared Bruni */

#include"cmx_joystick.h"
#include<iostream>
#include<windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>
#include<string>
#include<vector>


LPDIRECTINPUT8 di_dev;
std::string joy_name = "";
bool joyactive = false;

struct JoyInfo {
	LPDIRECTINPUTDEVICE8 joy_stick;
	bool joy_exisits;
	std::string joy_name;
};


std::vector<JoyInfo> joysticks;

BOOL CALLBACK EnumObjects(const DIDEVICEOBJECTINSTANCE* dioi, VOID* context) {
	if (dioi->dwType & DIDFT_AXIS) {
		DIPROPRANGE diprg;
		diprg.diph.dwSize = sizeof(DIPROPRANGE);
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		diprg.diph.dwHow = DIPH_BYID;
		diprg.diph.dwObj = dioi->dwType;
		diprg.lMin = -1000;
		diprg.lMax = +1000;

	LPDIRECTINPUTDEVICE8 cur = (LPDIRECTINPUTDEVICE8)context;

		if (FAILED(cur->SetProperty(DIPROP_RANGE, &diprg.diph)));
			return DIENUM_STOP;
	}
	return DIENUM_CONTINUE;
}

BOOL CALLBACK Enum(const DIDEVICEINSTANCE *joy_inst, VOID *context) {
	LPDIRECTINPUTDEVICE8 joy_stick;
	HRESULT hr = di_dev->CreateDevice(joy_inst->guidInstance, &joy_stick, NULL);
	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	} else {
		JoyInfo info = { joy_stick, true, joy_inst->tszProductName };
		joysticks.push_back(info);
		joyactive = true;
		std::cout << "Joystick: " << joy_inst->tszProductName << " connected.\n";
	}
	return DIENUM_CONTINUE;
}

void Init(HWND hwnd) {
	di_dev->EnumDevices(DI8DEVCLASS_GAMECTRL, Enum, NULL, DIEDFL_ATTACHEDONLY);
	if (joysticks.size() == 0) {
		joyactive = false;
		return;
	}
	for(int i = 0; i < joysticks.size(); ++i) {
	HRESULT rt = joysticks[i].joy_stick->SetDataFormat(&c_dfDIJoystick2);
	rt = joysticks[i].joy_stick->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(rt)) {
		MessageBox(hwnd, "Error setting of cooperative level for joystick failed!.\n", "Error", MB_ICONERROR);
		return;
	}

	rt = joysticks[i].joy_stick->EnumObjects(EnumObjects, (VOID*) joysticks[i].joy_stick, DIDFT_ALL);

	if (FAILED(rt)) {
		MessageBox(hwnd, "Failed Enumerating Objects", "Error", MB_ICONERROR);
		return;
	}

	}
}

namespace cmx {
	namespace input {

	void *extra;

		void close_joystick() {

			for(int i = 0; i < joysticks.size(); ++i) {
				if(joysticks[i].joy_stick != 0)
				joysticks[i].joy_stick->Unacquire();
				joysticks[i].joy_stick = 0;
			}


			if(di_dev) {
				di_dev->Release();
				di_dev = 0;
			}
		}
		void init_joystick() {
			 HRESULT rt;
			 rt = DirectInput8Create(GetModuleHandle(0),DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&di_dev,NULL);
			 if(FAILED(rt)) {
				 joyactive = false;
				 MessageBox(0, "Failed creating DirectX8Input", "Error", MB_ICONERROR);
				 return;
			 }
			 Init(*(HWND*)extra);
		}

		int num_joystick() {return (int)joysticks.size();}


		bool poll_joystick(int num, int btn) {

			if(num > joysticks.size() || num < 0)
				return false;

			if(joyactive == true) {

				HRESULT hr = joysticks[num].joy_stick->Poll();
				if(FAILED(hr)) {
					hr = joysticks[num].joy_stick->Acquire();
					while(hr == DIERR_INPUTLOST) {
						hr = joysticks[num].joy_stick->Acquire();
					}
					return false;
				}

				 DIJOYSTATE2 joy_st;
				 joysticks[num].joy_stick->GetDeviceState(sizeof(DIJOYSTATE2), &joy_st);

				 switch(btn) {
				 case BUTTON_LEFT:
					 if(joy_st.lX < -700)
						 return true;
					 break;
				 case BUTTON_RIGHT:
					 if(joy_st.lX > 700)
						 return true;
					 break;
				 case BUTTON_UP:
					 if(joy_st.lY > 700)
						 return true;
					 break;
				 case BUTTON_DOWN:
					 if(joy_st.lY < -700)
						 return true;
					 break;
				 }

				 int btn_val[] = { 0, 0, 0, 0, 0, 1, 2, 0, 3, 4, 5, 4, 7, 8, 9, 0, 0 };
				 if(btn >= BUTTON_CROSS && btn <= BUTTON_START) {
					 if(joy_st.rgbButtons[btn_val[btn]] & 0x80) return true;
				 } else return false;
			}
			return false;
		}
	}
}

