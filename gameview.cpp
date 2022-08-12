#include "pch.h"
#include "gameview.h"

SDL_Renderer* GameViewRenderer;

bool GameviewEnterWorld()
{
	if (!InitializeAllFlaps(640, 480))
	{
		return false;
	}

	PopulateAllFlaps();

	return true;
}

void GameViewRenderFrame()
{
	RenderFloor();
	RenderMobs();
	RenderWalls();
	RenderRooof();
}

void GameviewTeardown()
{
	RenderstateTeardown();
}