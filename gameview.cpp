#include "pch.h"
#include "gameview.h"
#include "gamestate.h"
#include "enums.h"
#include "settings.h"

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

// Offsets for the square mode (when the viewport is a square but the screen
// is a rectangle)
int viewportOffsetX = 0;
int viewportOffsetY = 0;

bool GameviewEnterWorld()
{
	const auto& viewWidth = blooDot::Settings::GetPhysicalArenaWidth();
	const auto& viewHight = blooDot::Settings::GetPhysicalArenaHeight();
	const auto& flapWidth = blooDot::Settings::GetLogicalArenaWidth();
	const auto& flapHight = blooDot::Settings::GetLogicalArenaHeight();

	viewportOffsetX = (viewWidth - flapWidth) / 2;
	viewportOffsetY = (viewHight - flapHight) / 2;

	if (!InitializeAllFlaps(flapWidth, flapHight))
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