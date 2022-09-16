#include "pch.h"
#include "orchestrator.h"
#include "menu-ingame.h"

extern SDL_Renderer* GameViewRenderer;

bool mainRunning = true;
SDL_Event mainEvent;

#ifndef NDEBUG
bool toggleDebugView = false;
#endif

void MainLoop(SDL_Renderer* renderer)
{
	if (!InitializeNewWorld())
	{
		mainRunning = false;
	}

	GameViewRenderer = renderer;
	if (mainRunning && !GameviewEnterWorld())
	{
		mainRunning = false;
	}

	while (mainRunning)
	{
		while (SDL_PollEvent(&mainEvent) != 0)
		{
			switch (mainEvent.type)
			{
			case SDL_KEYDOWN:
				switch (mainEvent.key.keysym.scancode)
				{
#ifndef NDEBUG
				case SDL_SCANCODE_LEFTBRACKET:
					/* this...is...the...bracket */
					toggleDebugView = !toggleDebugView;
					break;

#endif
				case SDL_SCANCODE_LEFT:
				case SDL_SCANCODE_KP_4:
					Scroll(-5, 0);
					break;

				case SDL_SCANCODE_RIGHT:
				case SDL_SCANCODE_KP_6:
					Scroll(5, 0);
					break;

				case SDL_SCANCODE_UP:
				case SDL_SCANCODE_KP_8:
					Scroll(0, -5);
					break;

				case SDL_SCANCODE_DOWN:
				case SDL_SCANCODE_KP_2:
					Scroll(0, 5);
					break;

				case SDL_SCANCODE_KP_7:
					Scroll(-5, -5);
					break;

				case SDL_SCANCODE_KP_9:
					Scroll(5, -5);
					break;

				case SDL_SCANCODE_KP_1:
					Scroll(-5, 5);
					break;

				case SDL_SCANCODE_KP_3:
					Scroll(5, 5);
					break;

				case SDL_SCANCODE_ESCAPE:
					blooDot::MenuInGame::MenuLoop(renderer);
					break;
				}

				break;

			case SDL_QUIT:
				mainRunning = false;
				break;
			}
		}

		SDL_RenderClear(renderer);
		GameViewRenderFrame();
		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}

	TeardownDingSheets();
	GameviewTeardown();
	ClearWorldData();
}