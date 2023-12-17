#pragma once
#include "SDL2/SDL.h"

namespace blooDot::ControllerStuff
{
	typedef struct ControllerStateStruct {
		bool A;
		bool B;
		bool X;
		bool Y;
		bool DL;
		bool DR;
		bool DD;
		bool DU;
		bool P1;
		bool LSH;
		bool LST;
		bool Back;
		bool Guide;
		bool Start;
		bool RST;
		bool RSH;
		bool P2;
	} ControllerState;

	extern std::string controller1Name;
	extern std::string controller2Name;
	extern std::string controller3Name;
	extern std::string controller4Name;

	extern SDL_JoystickID controller1Connected;
	extern SDL_JoystickID controller2Connected;
	extern SDL_JoystickID controller3Connected;
	extern SDL_JoystickID controller4Connected;

	extern SDL_GameController* controller1;
	extern SDL_GameController* controller2;
	extern SDL_GameController* controller3;
	extern SDL_GameController* controller4;

	ControllerState controllerState[];

	bool Initialize(SDL_Renderer*);
	void StatusQuo();
	bool OnControllerDeviceConnected(Sint32 deviceIndex);
	bool OnControllerDeviceRemoved(Sint32 instanceId);
	bool RegisterButtonState(Sint32 instanceId, SDL_GameControllerButton which, bool state);
	void Teardown();
}