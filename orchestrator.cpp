#include "pch.h"
#include "orchestrator.h"

bool mainRunning = true;
SDL_Event mainEvent;

void MainLoop(SDL_Renderer* renderer)
{
	char i = 0;
	while (mainRunning)
	{
		while (SDL_PollEvent(&mainEvent) != 0)
		{
			switch (mainEvent.type)
			{
			case SDL_QUIT:
				mainRunning = false;
				break;
			}
		}

		SDL_SetRenderDrawColor(renderer, i, 30, 28, 200);
		i = ++i % 0xff;

		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}
}