#include "pch.h"
#include "renderstate.h"
#include <iostream>

constexpr unsigned short flapIndicesHorz[] = { 3,4,5 };
constexpr unsigned short flapIndicesVert[] = { 1,4,7 };

extern SDL_Renderer* GameViewRenderer;

extern int originDx;
extern int originDy;

int flapW = 0;
int flapH = 0;
int halfW = 0;
int halfH = 0;

SDL_Texture* flapsFloor[9];
SDL_Texture* flapsWalls[9];
SDL_Texture* flapsRooof[9];

/* contains the indices to the flaps,
 * where its own indices are position-stable.
 * the flaps swap around, so this is used
 * to keep track */
unsigned short flapIndirection[9];

/* the sheet numbers are the indices to flapIndirection.
 * floor, walls and rooof are always swapped together
 * and therefore automatically in sync among themselves

  ┌─────────┬─────────┬─────────┐
  │0        │1        │2        │
  │         │      ┏━━┿━━━━━┓   │
  ├─────────┼──────╂──┼─────╂───┤
  │3        │4     ┃VP│5    ┃-> │
  │         │      ┗━━│━━━━━┛   │
  ├─────────┼─────────┼─────────┤
  │6        │7        │8        │
  │         │         │         │
  └─────────┴─────────┴─────────┘

* VP is viewport.
* I foresee that this unnecessarily complicated approach
* will be a source of glitches to no end
*/

FlappySituation constellation = FS_BUNGHOLE;
SDL_Rect bunghole;
SDL_Rect vertUpperSrc;
SDL_Rect vertUpperDst;
SDL_Rect vertLowerSrc;
SDL_Rect vertLowerDst;
SDL_Rect horzLeftSrc;
SDL_Rect horzLeftDst;
SDL_Rect horzRiteSrc;
SDL_Rect horzRiteDst;
SDL_Rect quadrantNWSrc;
SDL_Rect quadrantNWDst;
SDL_Rect quadrantNESrc;
SDL_Rect quadrantNEDst;
SDL_Rect quadrantSWSrc;
SDL_Rect quadrantSWDst;
SDL_Rect quadrantSESrc;
SDL_Rect quadrantSEDst;
unsigned short vertUpperLower = 0;
unsigned short horzLeftRight = 0;
unsigned short flapIndexNW; // only relevant in the quadrant topology
unsigned short flapIndexNE;
unsigned short flapIndexSW;
unsigned short flapIndexSE;

SDL_Texture* NewTexture(SDL_Renderer* renderer, bool transparentAble)
{
	const auto newTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		flapW,
		flapH
	);

	if (!newTexture)
	{
		const auto creationError = SDL_GetError();
		ReportError("Failed to create flap texture", creationError);
	}

	if (transparentAble)
	{
		if (SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND) < 0)
		{
			const auto blendModeError = SDL_GetError();
			ReportError("Failed to set flap texture blend mode", blendModeError);
		}
	}

	return newTexture;
}

bool InitializeAllFlaps(int width, int height)
{
	flapW = width;
	halfW = width / 2;
	flapH = height;
	halfH = height / 2;

	/* the centerpiece is constantly identical to the viewport */
	bunghole = SDL_Rect{ 0,0,width,height };

	for (auto i = 0; i < 9; ++i)
	{
		flapsFloor[i] = NewTexture(GameViewRenderer, false);
		if (!flapsFloor[i]) 
		{
			return false;
		}

		flapsWalls[i] = NewTexture(GameViewRenderer, true);
		if (!flapsWalls[i])
		{
			return false;
		}

		flapsRooof[i] = NewTexture(GameViewRenderer, true);
		if (!flapsRooof[i])
		{
			return false;
		}
	}

	RecomputeFlapConstellation();
	RecomputeFlapRects();

	return true;
}

void RecomputeFlapConstellation()
{
	const bool isVerticallyAligned = originDx % flapW == 0;
	const bool isHorizntalyAligned = originDy % flapH == 0;
	if (isVerticallyAligned && isHorizntalyAligned)
	{
		constellation = FS_BUNGHOLE;
#ifndef NDEBUG
		std::cout << "Are you threatening me?";
#endif
	}
	else if (isVerticallyAligned)
	{
		constellation = FS_VERTICAL;
	}
	else if (isHorizntalyAligned)
	{
		constellation = FS_HORIZONTAL;
	}
	else
	{
		constellation = FS_QUARTERED;
	}
}

void RecomputeFlapRects()
{
	switch (constellation)
	{
	case FS_BUNGHOLE:
		/* nothing to compute, this is always the centerpiece (flap 4) */
		break;

	case FS_VERTICAL:

		break;

	case FS_HORIZONTAL:

		break;

	default:

		break;
	}
}

void PopulateAllFlaps()
{
	for (unsigned short i = 0; i < 9; ++i)
	{
		flapIndirection[i] = i;
		PopulateFlap(i);
	}
}

void PopulateTopRowFlaps()
{
	PopulateFlap(0);
	PopulateFlap(1);
	PopulateFlap(2);
}

void PopulateFlap(int flapIndex)
{
	const auto textureIndex = flapIndirection[flapIndex];
	const auto floor = flapsFloor[textureIndex];
	const auto walls = flapsWalls[textureIndex];
	const auto rooof = flapsRooof[textureIndex];

#ifndef NDEBUG
	auto drawingSink = BeginTextureDrawing(floor);

	cairo_set_source_rgb(drawingSink, 1, 0.68, 0.08);
		
	cairo_move_to(drawingSink, 0, 16);
	cairo_line_to(drawingSink, 0, 0);
	cairo_line_to(drawingSink, 16, 0);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, flapW - 16, 0);
	cairo_line_to(drawingSink, flapW, 0);
	cairo_line_to(drawingSink, flapW, 16);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, flapW - 16, flapH);
	cairo_line_to(drawingSink, flapW, flapH);
	cairo_line_to(drawingSink, flapW, flapH - 16);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, 0, flapH - 16);
	cairo_line_to(drawingSink, 0, flapH);
	cairo_line_to(drawingSink, 16, flapH);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, 10, 16);
	cairo_text_path(drawingSink, std::to_string(flapIndex).c_str());
	cairo_move_to(drawingSink, 25, 16);
	cairo_text_path(drawingSink, std::to_string(textureIndex).c_str());

	cairo_fill(drawingSink);

	EndTextureDrawing(floor, drawingSink);
#endif

#ifndef NDEBUG
	drawingSink = BeginTextureDrawing(walls);

	cairo_set_source_rgb(drawingSink, 0.68, 1, 0.08);

	cairo_move_to(drawingSink, 1, 15);
	cairo_line_to(drawingSink, 1, 1);
	cairo_line_to(drawingSink, 15, 1);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, flapW - 15, 1);
	cairo_line_to(drawingSink, flapW - 1, 1);
	cairo_line_to(drawingSink, flapW - 1, 15);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, flapW - 16, flapH - 1);
	cairo_line_to(drawingSink, flapW - 1, flapH - 1);
	cairo_line_to(drawingSink, flapW - 1, flapH - 15);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, 1, flapH - 15);
	cairo_line_to(drawingSink, 1, flapH - 1);
	cairo_line_to(drawingSink, 15, flapH - 1);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, 20, 26);
	cairo_text_path(drawingSink, std::to_string(flapIndex).c_str());
	cairo_move_to(drawingSink, 35, 26);
	cairo_text_path(drawingSink, std::to_string(textureIndex).c_str());

	cairo_fill(drawingSink);

	EndTextureDrawing(walls, drawingSink);
#endif

#ifndef NDEBUG
	drawingSink = BeginTextureDrawing(rooof);

	cairo_set_source_rgb(drawingSink, 0.08, 0.68, 1);

	cairo_move_to(drawingSink, 2, 14);
	cairo_line_to(drawingSink, 2, 2);
	cairo_line_to(drawingSink, 14, 2);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, flapW - 14, 2);
	cairo_line_to(drawingSink, flapW - 2, 2);
	cairo_line_to(drawingSink, flapW - 2, 14);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, flapW - 14, flapH - 2);
	cairo_line_to(drawingSink, flapW - 2, flapH - 2);
	cairo_line_to(drawingSink, flapW - 2, flapH - 14);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, 2, flapH - 14);
	cairo_line_to(drawingSink, 2, flapH - 2);
	cairo_line_to(drawingSink, 14, flapH - 2);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, 30, 36);
	cairo_text_path(drawingSink, std::to_string(flapIndex).c_str());
	cairo_move_to(drawingSink, 45, 36);
	cairo_text_path(drawingSink, std::to_string(textureIndex).c_str());

	cairo_fill(drawingSink);

	EndTextureDrawing(rooof, drawingSink);
#endif
}

/// <summary>
/// The top row becomes the bottom row,
/// the middle and bottom move to the top,
/// the new bottom row is scheduled for repopulation
/// </summary>
void FlapoverDown()
{
	const auto topLft = flapIndirection[0];
	const auto topMid = flapIndirection[1];
	const auto topRgt = flapIndirection[2];
	flapIndirection[0] = flapIndirection[3];
	flapIndirection[1] = flapIndirection[4];
	flapIndirection[2] = flapIndirection[5];
	flapIndirection[3] = flapIndirection[6];
	flapIndirection[4] = flapIndirection[7];
	flapIndirection[5] = flapIndirection[8];
	flapIndirection[6] = topLft;
	flapIndirection[7] = topMid;
	flapIndirection[8] = topRgt;
}

/// <summary>
/// The bottom row becomes the top row,
/// the middle and top move to the bottom,
/// the new top row is scheduled for repopulation
/// </summary>
void FlapoverUp()
{
	const auto bottomLft = flapIndirection[6];
	const auto bottomMid = flapIndirection[7];
	const auto bottomRgt = flapIndirection[8];
	flapIndirection[6] = flapIndirection[3];
	flapIndirection[7] = flapIndirection[4];
	flapIndirection[8] = flapIndirection[5];
	flapIndirection[3] = flapIndirection[0];
	flapIndirection[4] = flapIndirection[1];
	flapIndirection[5] = flapIndirection[2];
	flapIndirection[0] = bottomLft;
	flapIndirection[1] = bottomMid;
	flapIndirection[2] = bottomRgt;
	PopulateTopRowFlaps();
}

/// <summary>
/// The right column becomes the leftmost column,
/// the middle and left move on to the right,
/// the new leftmost column is scheduled for repopulation
/// </summary>
void FlapoverLeft()
{
	const auto rightTop = flapIndirection[2];
	const auto rightMid = flapIndirection[5];
	const auto rightBot = flapIndirection[8];
	flapIndirection[2] = flapIndirection[1];
	flapIndirection[5] = flapIndirection[4];
	flapIndirection[8] = flapIndirection[7];
	flapIndirection[1] = flapIndirection[0];
	flapIndirection[4] = flapIndirection[3];
	flapIndirection[7] = flapIndirection[6];
	flapIndirection[0] = rightTop;
	flapIndirection[3] = rightMid;
	flapIndirection[6] = rightBot;
}

/// <summary>
/// The left column becomes the rightmost column,
/// the middle and right move one to the left,
/// the new rightmost column is scheduled for repopulation
/// (this is the scenario depicted in the drawing)
/// </summary>
void FlapoverRight()
{
	const auto leftTop = flapIndirection[0];
	const auto leftMid = flapIndirection[3];
	const auto leftBot = flapIndirection[6];
	flapIndirection[0] = flapIndirection[1];
	flapIndirection[3] = flapIndirection[4];
	flapIndirection[6] = flapIndirection[7];
	flapIndirection[1] = flapIndirection[2];
	flapIndirection[4] = flapIndirection[5];
	flapIndirection[7] = flapIndirection[8];
	flapIndirection[2] = leftTop;
	flapIndirection[5] = leftMid;
	flapIndirection[8] = leftBot;
}

void RenderFloorBung()
{	
	SDL_RenderCopy(GameViewRenderer, flapsFloor[BUNGHOLE], &bunghole, &bunghole);
}

void RenderFloorVert()
{
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndicesVert[vertUpperLower]], &vertUpperSrc, &vertUpperDst);
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndicesVert[vertUpperLower + 1]], &vertLowerSrc, &vertLowerDst);
}

void RenderFloorHorz()
{
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndicesHorz[horzLeftRight]], &horzLeftSrc, &horzLeftDst);
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndicesHorz[horzLeftRight + 1]], &horzRiteSrc, &horzRiteDst);
}

void RenderFloorQuart()
{
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndexNW], &quadrantNWSrc, &quadrantNWDst);
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndexNE], &quadrantNESrc, &quadrantNEDst);
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndexSW], &quadrantSWSrc, &quadrantSWDst);
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndexSE], &quadrantSESrc, &quadrantSEDst);
}

void RenderMobs()
{

}

void RenderWallsAndRooofBung()
{
	SDL_RenderCopy(GameViewRenderer, flapsWalls[BUNGHOLE], &bunghole, &bunghole);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[BUNGHOLE], &bunghole, &bunghole);
}

void RenderWallsAndRooofVert()
{
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndicesVert[vertUpperLower]], &vertUpperSrc, &vertUpperDst);
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndicesVert[vertUpperLower + 1]], &vertLowerSrc, &vertLowerDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndicesVert[vertUpperLower]], &vertUpperSrc, &vertUpperDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndicesVert[vertUpperLower + 1]], &vertLowerSrc, &vertLowerDst);
}

void RenderWallsAndRooofHorz()
{
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndicesHorz[horzLeftRight]], &horzLeftSrc, &horzLeftDst);
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndicesHorz[horzLeftRight + 1]], &horzRiteSrc, &horzRiteDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndicesHorz[horzLeftRight]], &horzLeftSrc, &horzLeftDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndicesHorz[horzLeftRight + 1]], &horzRiteSrc, &horzRiteDst);
}

void RenderWallsAndRooofQuart()
{
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndexNW], &quadrantNWSrc, &quadrantNWDst);
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndexNE], &quadrantNESrc, &quadrantNEDst);
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndexSW], &quadrantSWSrc, &quadrantSWDst);
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndexSE], &quadrantSESrc, &quadrantSEDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndexNW], &quadrantNWSrc, &quadrantNWDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndexNE], &quadrantNESrc, &quadrantNEDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndexSW], &quadrantSWSrc, &quadrantSWDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndexSE], &quadrantSESrc, &quadrantSEDst);
}

void RenderstateTeardown()
{
	for (auto i = 0; i < 9; ++i)
	{
		SDL_DestroyTexture(flapsFloor[i]);
		SDL_DestroyTexture(flapsWalls[i]);
		SDL_DestroyTexture(flapsRooof[i]);
	}
}