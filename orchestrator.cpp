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
				switch (mainEvent.key.keysym.sym)
				{
				case SDLK_LEFT:
					Scroll(-5, 0);
					break;

				case SDLK_RIGHT:
					Scroll(5, 0);
					break;

				case SDLK_UP:
					Scroll(0, -5);
					break;

				case SDLK_DOWN:
					Scroll(0, 5);
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