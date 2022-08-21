#include "pch.h"
#include "gameview.h"
#include "gamestate.h"
#include "enums.h"

/* the gameview knows where we are in the world.
 * the gamestate tracks mobs and other entities
 * the renderstate takes care of the scroll flapping */

extern FlappySituation constellation;

SDL_Renderer* GameViewRenderer;

// current absolute scroll distance from the world origin centerpoint, hwich
// can and will go negative for locations left and up relative to the origin
// and which is one of the things that is stored with the game state on save.
int originDx = 0;
int originDy = 0;

bool GameviewEnterWorld()
{
	if (!InitializeAllFlaps(640, 480))
	{
		return false;
	}

	PopulateAllFlaps();

#ifndef NDEBUG
	std::cout
		<< ">> "
		<< GetRegionName(0, 0)
		<< " <<\n";
#endif

	return true;
}

void GameViewRenderFrame()
{
	switch (constellation)
	{
	case FS_QUARTERED:
		RenderFloorQuart();
		RenderMobs();
		RenderWallsAndRooofQuart();
		break;

	case FS_VERTICAL:
		RenderFloorVert();
		RenderMobs();
		RenderWallsAndRooofVert();
		break;

	case FS_HORIZONTAL:
		RenderFloorHorz();
		RenderMobs();
		RenderWallsAndRooofHorz();
		break;

	default:
		/* the least likely */
		RenderFloorBung();
		RenderMobs();
		RenderWallsAndRooofBung();
		break;
	}
}

void GameviewTeardown()
{
	RenderstateTeardown();
}