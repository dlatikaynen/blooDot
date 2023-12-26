#include "pch.h"
#include "controller-stuff.h"

namespace blooDot::ControllerStuff
{
	std::string controller1Name;
	std::string controller2Name;
	std::string controller3Name;
	std::string controller4Name;

	SDL_JoystickID controller1Connected = -1;
	SDL_JoystickID controller2Connected = -1;
	SDL_JoystickID controller3Connected = -1;
	SDL_JoystickID controller4Connected = -1;

	SDL_GameController* controller1 = nullptr;
	SDL_GameController* controller2 = nullptr;
	SDL_GameController* controller3 = nullptr;
	SDL_GameController* controller4 = nullptr;

	ControllerState controllerState[4] = { {0},{0},{0},{0} };

	bool Initialize(SDL_Renderer*)
	{
		return true;
	}

	bool IsAnyControllerConnected()
	{
		return controller1 || controller2 || controller3 || controller4;
	}

	void StatusQuo()
	{
		auto numJoysticks = SDL_NumJoysticks();

		for (auto i = 0; i < numJoysticks; ++i)
		{
			auto instanceId = SDL_JoystickGetDeviceInstanceID(i);

			if (instanceId >= 0)
			{
				auto playerIndex = SDL_JoystickGetDevicePlayerIndex(instanceId);

				if (playerIndex == 1 && controller1 == nullptr)
				{
					controller1Connected = instanceId;
					controller1 = SDL_GameControllerOpen(i);
					controller1Name.assign(SDL_GameControllerNameForIndex(i));
				}
				else if (playerIndex == 2 && controller2 == nullptr)
				{
					controller2Connected = instanceId;
					controller2 = SDL_GameControllerOpen(i);
					controller2Name.assign(SDL_GameControllerNameForIndex(i));
				}
				else if (playerIndex == 3 && controller3 == nullptr)
				{
					controller3Connected = instanceId;
					controller3 = SDL_GameControllerOpen(i);
					controller3Name.assign(SDL_GameControllerNameForIndex(i));
				}
				else if (playerIndex == 4 && controller4 == nullptr)
				{
					controller4Connected = instanceId;
					controller4 = SDL_GameControllerOpen(i);
					controller4Name.assign(SDL_GameControllerNameForIndex(i));
				}
			}
		}

		// controllers not identifying as players get the leftovers
		for (auto i = 0; i < numJoysticks; ++i)
		{
			auto instanceId = SDL_JoystickGetDeviceInstanceID(i);

			if (instanceId >= 0)
			{
				auto playerIndex = SDL_JoystickGetDevicePlayerIndex(instanceId);

				if (playerIndex <= 0)
				{
					if (controller1Connected == -1)
					{
						controller1Connected = instanceId;
						controller1 = SDL_GameControllerOpen(i);
						controller1Name.assign(SDL_GameControllerNameForIndex(i));
					}
					else if (controller2Connected == -1)
					{
						controller2Connected = instanceId;
						controller2 = SDL_GameControllerOpen(i);
						controller2Name.assign(SDL_GameControllerNameForIndex(i));
					}
					else if (controller3Connected == -1)
					{
						controller3Connected = instanceId;
						controller3 = SDL_GameControllerOpen(i);
						controller3Name.assign(SDL_GameControllerNameForIndex(i));
					}
					else if (controller4Connected == -1)
					{
						controller4Connected = instanceId;
						controller4 = SDL_GameControllerOpen(i);
						controller4Name.assign(SDL_GameControllerNameForIndex(i));
					}
				}
			}
		}
	}

	bool OnControllerDeviceConnected(Sint32 deviceIndex)
	{
		auto instanceId = SDL_JoystickGetDeviceInstanceID(deviceIndex);

		if (instanceId < 0)
		{
			return false;
		}

		auto playerIndex = SDL_JoystickGetDevicePlayerIndex(instanceId);
		auto needRedraw = false;

		if (playerIndex <= 0)
		{
			if (controller1Connected == -1)
			{
				controller1Connected = instanceId;
				// TODO: handle open errors
				controller1 = SDL_GameControllerOpen(deviceIndex);
				controller1Name.assign(SDL_GameControllerNameForIndex(deviceIndex));
				needRedraw = true;
			}
			else if (controller2Connected == -1)
			{
				controller2Connected = instanceId;
				controller2 = SDL_GameControllerOpen(deviceIndex);
				controller2Name.assign(SDL_GameControllerNameForIndex(deviceIndex));
				needRedraw = true;
			}
			else if (controller3Connected == -1)
			{
				controller3Connected = instanceId;
				controller3 = SDL_GameControllerOpen(deviceIndex);
				controller3Name.assign(SDL_GameControllerNameForIndex(deviceIndex));
				needRedraw = true;
			}
			else if (controller4Connected == -1)
			{
				controller4Connected = instanceId;
				controller4 = SDL_GameControllerOpen(deviceIndex);
				controller4Name.assign(SDL_GameControllerNameForIndex(deviceIndex));
				needRedraw = true;
			}
		}
		else if (playerIndex == 1)
		{
			controller1Connected = instanceId;
			controller1 = SDL_GameControllerOpen(deviceIndex);
			controller1Name.assign(SDL_GameControllerNameForIndex(deviceIndex));
			needRedraw = true;
		}
		else if (playerIndex == 2)
		{
			controller2Connected = instanceId;
			controller2 = SDL_GameControllerOpen(deviceIndex);
			controller2Name.assign(SDL_GameControllerNameForIndex(deviceIndex));
			needRedraw = true;
		}
		else if (playerIndex == 3)
		{
			controller3Connected = instanceId;
			controller3 = SDL_GameControllerOpen(deviceIndex);
			controller3Name.assign(SDL_GameControllerNameForIndex(deviceIndex));
			needRedraw = true;
		}
		else if (playerIndex == 4)
		{
			controller4Connected = instanceId;
			controller4 = SDL_GameControllerOpen(deviceIndex);
			controller4Name.assign(SDL_GameControllerNameForIndex(deviceIndex));
			needRedraw = true;
		}

		return needRedraw;
	}

	bool OnControllerDeviceRemoved(Sint32 instanceId)
	{
		auto needRedraw = false;

		if (controller1Connected == instanceId)
		{
			controller1Connected = -1;
			if (controller1 != nullptr)
			{
				SDL_GameControllerClose(controller1);
				controller1 = nullptr;
			}

			controller1Name.clear();
			needRedraw = true;
		}

		if (controller2Connected == instanceId)
		{
			controller2Connected = -1;
			if (controller2 != nullptr)
			{
				SDL_GameControllerClose(controller2);
				controller2 = nullptr;
			}

			controller2Name.clear();
			needRedraw = true;
		}

		if (controller3Connected == instanceId)
		{
			controller3Connected = -1;
			if (controller3 != nullptr)
			{
				SDL_GameControllerClose(controller3);
				controller3 = nullptr;
			}

			controller3Name.clear();
			needRedraw = true;
		}

		if (controller4Connected == instanceId)
		{
			controller4Connected = -1;
			if (controller4 != nullptr)
			{
				SDL_GameControllerClose(controller4);
				controller4 = nullptr;
			}

			controller4Name.clear();
			needRedraw = true;
		}

		return needRedraw;
	}

	bool RegisterButtonState(Sint32 instanceId, SDL_GameControllerButton which, bool state)
	{
		auto changed = false;
		auto playerIndex = 0;

		if (instanceId == controller1Connected)
		{
			playerIndex = 1;
		}
		else if (instanceId == controller2Connected)
		{
			playerIndex = 2;
		}
		else if (instanceId == controller3Connected)
		{
			playerIndex = 3;
		}
		else if (instanceId == controller4Connected)
		{
			playerIndex = 4;
		}

		if (playerIndex == 0)
		{
			return false;
		}

		auto stateIndex = playerIndex - 1;

		switch (which)
		{
		case ::SDL_CONTROLLER_BUTTON_A:
			if (controllerState[stateIndex].A != state)
			{
				controllerState[stateIndex].A = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_B:
			if (controllerState[stateIndex].B != state)
			{
				controllerState[stateIndex].B = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_X:
			if (controllerState[stateIndex].X != state)
			{
				controllerState[stateIndex].X = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_Y:
			if (controllerState[stateIndex].Y != state)
			{
				controllerState[stateIndex].Y = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			if (controllerState[stateIndex].DL != state)
			{
				controllerState[stateIndex].DL = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			if (controllerState[stateIndex].DR != state)
			{
				controllerState[stateIndex].DR = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_DPAD_DOWN:
			if (controllerState[stateIndex].DD != state)
			{
				controllerState[stateIndex].DD = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_DPAD_UP:
			if (controllerState[stateIndex].DU != state)
			{
				controllerState[stateIndex].DU = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_PADDLE1:
		case ::SDL_CONTROLLER_BUTTON_PADDLE4:
			if (controllerState[stateIndex].P1 != state)
			{
				controllerState[stateIndex].P1 = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
			if (controllerState[stateIndex].LSH != state)
			{
				controllerState[stateIndex].LSH = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_LEFTSTICK:
			if (controllerState[stateIndex].LST != state)
			{
				controllerState[stateIndex].LST = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_BACK:
			if (controllerState[stateIndex].Back != state)
			{
				controllerState[stateIndex].Back = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_GUIDE:
			if (controllerState[stateIndex].Guide != state)
			{
				controllerState[stateIndex].Guide = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_START:
			if (controllerState[stateIndex].Start != state)
			{
				controllerState[stateIndex].Start = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_RIGHTSTICK:
			if (controllerState[stateIndex].RST != state)
			{
				controllerState[stateIndex].RST = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
			if (controllerState[stateIndex].RSH != state)
			{
				controllerState[stateIndex].RSH = state;
				changed = true;
			}

			break;

		case ::SDL_CONTROLLER_BUTTON_PADDLE2:
		case ::SDL_CONTROLLER_BUTTON_PADDLE3:
			if (controllerState[stateIndex].P2 != state)
			{
				controllerState[stateIndex].P2 = state;
				changed = true;
			}

			break;
		}

		return changed;
	}

	void Teardown()
	{
		if (controller1 != nullptr)
		{
			SDL_GameControllerClose(controller1);
			controller1 = nullptr;
		}

		if (controller2 != nullptr)
		{
			SDL_GameControllerClose(controller2);
			controller2 = nullptr;
		}

		if (controller3 != nullptr)
		{
			SDL_GameControllerClose(controller3);
			controller3 = nullptr;
		}

		if (controller4 != nullptr)
		{
			SDL_GameControllerClose(controller4);
			controller4 = nullptr;
		}
	}
}
