#include "pch.h"
#include "orchestrator.h"

extern SDL_Renderer* GameViewRenderer;

bool mainRunning = true;
SDL_Event mainEvent;

void MainLoop(SDL_Renderer* renderer)
{
	GameViewRenderer = renderer;
	if (!GameviewEnterWorld())
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

	GameviewTeardown();
}