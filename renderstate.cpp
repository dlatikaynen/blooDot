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
		flapsFloor[i] = _NewTexture(GameViewRenderer, false);
		if (!flapsFloor[i]) 
		{
			return false;
		}

		flapsWalls[i] = _NewTexture(GameViewRenderer, true);
		if (!flapsWalls[i])
		{
			return false;
		}

		flapsRooof[i] = _NewTexture(GameViewRenderer, true);
		if (!flapsRooof[i])
		{
			return false;
		}
	}

	RecomputeFlapConstellation();

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

void Scroll(int dx, int dy)
{
	const bool isHorz = dy == 0;
	const bool isVert = dx == 0;

	originDx += dx;
	originDy += dy;

	const bool isVertSnapped = originDx % flapW == 0;
	const bool isHorzSnapped = originDy % flapH == 0;

	switch (constellation)
	{
	case FS_BUNGHOLE:
		if (isHorz)
		{
			/* break out into a vertically locked (horizontal-only) slide */
			if (dx > 0)
			{
				horzLeftRight = 1;
				horzLeftSrc = { dx,0,flapW - dx,flapH };
				horzLeftDst = { 0,0,flapW - dx,flapH };
				horzRiteSrc = { 0,0,dx,flapH };
				horzRiteDst = { flapW - dx,0,dx,flapH };
			}
			else
			{
				horzLeftRight = 0;
				horzLeftSrc = { flapW + dx,0,-dx,flapH };
				horzLeftDst = { 0,0,-dx,flapH };
				horzRiteSrc = { 0,0,flapW + dx,flapH };
				horzRiteDst = { -dx,0,flapW+dx,flapH };
			}

			constellation = FS_HORIZONTAL;
		}
		else if (isVert)
		{
			/* break out into a horizontally locked (vertical-only) slide */
			if (dy > 0)
			{
				vertUpperLower = 1;
				vertUpperSrc = { 0,dy,flapW,flapH - dy };
				vertUpperDst = { 0,0,flapW,flapH - dy };
				vertLowerSrc= { 0,0,flapW,dy };
				vertLowerDst = { 0,flapH - dy,flapW,dy };
			}
			else
			{
				vertUpperLower = 0;
				vertUpperSrc = { 0,flapH + dy,flapW,-dy };
				vertUpperDst = { 0,0,flapW,-dy };
				vertLowerSrc = { 0,0,flapW,flapH+dy };
				vertLowerDst = { 0,-dy,flapW,flapH + dy };
			}

			constellation = FS_VERTICAL;
		}
		else
		{
			/* break out into one of the four quadrant slides */
			if (dx < 0) 
			{
				if (dy < 0)
				{
					/* NW */
					flapIndexNW = 0; flapIndexNE = 1; flapIndexSW = 3; flapIndexSE = 4;
				}
				else
				{
					/* SW */
					flapIndexNW = 3; flapIndexNE = 4; flapIndexSW = 6; flapIndexSE = 7;
				}
			}
			else
			{
				if (dy < 0)
				{
					/* NE */
					flapIndexNW = 1; flapIndexNE = 2; flapIndexSW = 4; flapIndexSE = 5;
				}
				else
				{
					/* SE */
					flapIndexNW = 4; flapIndexNE = 5; flapIndexSW = 7; flapIndexSE = 8;
				}
			}

			constellation = FS_QUARTERED;
		}

		break;

	case FS_HORIZONTAL:
		if (isHorz)
		{
			if (isVertSnapped)
			{
				constellation = FS_BUNGHOLE;
			}
			else
			{
				horzLeftSrc.x += dx;
				horzLeftSrc.w -= dx;
				horzLeftDst.w -= dx;
				horzRiteSrc.w += dx;
				horzRiteDst.x -= dx;
				horzRiteDst.w += dx;
			}
		}
		else 
		{
			/* break out into a quarter freeform or vertical lock */
			if (isVertSnapped)
			{
				if (dy > 0)
				{
					vertUpperLower = 1;
				}
				else
				{
					vertUpperLower = 0;
				}

				constellation = FS_VERTICAL;
			}
			else
			{
				if (dy < 0)
				{
					if (horzLeftRight == 1) 
					{
						/* NE */
						flapIndexNW = 1; flapIndexNE = 2; flapIndexSW = 4; flapIndexSE = 5;
					}
					else
					{
						/* NW */
						flapIndexNW = 0; flapIndexNE = 1; flapIndexSW = 3; flapIndexSE = 4;
					}
				}
				else
				{
					if (horzLeftRight == 1)
					{
						/* SE */
						flapIndexNW = 4; flapIndexNE = 5; flapIndexSW = 7; flapIndexSE = 8;
					}
					else
					{
						/* SW */
						flapIndexNW = 3; flapIndexNE = 4; flapIndexSW = 6; flapIndexSE = 7;
					}
				}

				constellation = FS_QUARTERED;
			}
		}

		break;

	case FS_VERTICAL:
		if (isVert)
		{
			if (isHorzSnapped)
			{
				constellation = FS_BUNGHOLE;
			}
			else
			{
				vertUpperSrc.y += dy;
				vertUpperSrc.h -= dy;
				vertUpperDst.h -= dy;
				vertLowerSrc.h += dy;
				vertLowerDst.y -= dy;
				vertLowerDst.h += dy;
			}
		}
		else
		{
			/* break out into a quarter freeform or a horizontal lock */
			if (isHorzSnapped)
			{
				if (dx > 0)
				{
					horzLeftRight = 1;
				}
				else
				{
					horzLeftRight = 0;
				}

				constellation = FS_HORIZONTAL;
			}
			else
			{
				if (dx < 0)
				{
					if (vertUpperLower == 1)
					{
						/* SW */
						flapIndexNW = 3; flapIndexNE = 4; flapIndexSW = 6; flapIndexSE = 7;
					}
					else
					{
						/* NW */
						flapIndexNW = 0; flapIndexNE = 1; flapIndexSW = 3; flapIndexSE = 4;
					}
				}
				else 
				{
					if (vertUpperLower == 1)
					{
						/* SW */
						flapIndexNW = 3; flapIndexNE = 4; flapIndexSW = 6; flapIndexSE = 7;
					}
					else
					{
						/* NE */
						flapIndexNW = 1; flapIndexNE = 2; flapIndexSW = 4; flapIndexSE = 5;
					}
				}

				constellation = FS_QUARTERED;
			}
		}

		break;

	default:
		assert(constellation == FS_QUARTERED);

		if (isVertSnapped && isHorzSnapped)
		{
			/* jackpot */
			constellation = FS_BUNGHOLE;
		}
		else if (isVertSnapped)
		{
			/* freerange to vertically gliding */
			constellation = FS_VERTICAL;
		}
		else if (isHorzSnapped)
		{
			/* freerange to horizontally gliding */
			constellation = FS_HORIZONTAL;
		}
		else
		{
			/* remains freeform */

		}

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

void PopulateBottomRowFlaps()
{
	PopulateFlap(6);
	PopulateFlap(7);
	PopulateFlap(8);
}

void PopulateLeftColFlaps() {
	PopulateFlap(0);
	PopulateFlap(3);
	PopulateFlap(6);
}

void PopulateRiteColFlaps() {
	PopulateFlap(2);
	PopulateFlap(5);
	PopulateFlap(8);
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

	if (constellation == FS_VERTICAL)
	{
		vertUpperLower = 0;
	} 
	else if (constellation == FS_QUARTERED)
	{
		flapIndexNW = flapIndexNW == 3 ? 0 : 1;
		flapIndexNE = flapIndexNE == 4 ? 1 : 2;
		flapIndexSW = flapIndexSW == 6 ? 3 : 4;
		flapIndexSE = flapIndexSE == 7 ? 4 : 5;
	}

	PopulateBottomRowFlaps();
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

	if (constellation == FS_VERTICAL)
	{
		vertUpperLower = 1;
	}
	else if (constellation == FS_QUARTERED)
	{
		flapIndexNW = flapIndexNW == 0 ? 3 : 4;
		flapIndexNE = flapIndexNE == 1 ? 4 : 5;
		flapIndexSW = flapIndexSW == 3 ? 6 : 7;
		flapIndexSE = flapIndexSE == 4 ? 7 : 8;
	}

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

	if (constellation == FS_HORIZONTAL)
	{
		horzLeftRight = 1;
	}
	else if (constellation == FS_QUARTERED)
	{
		flapIndexNW = flapIndexNW == 0 ? 1 : 4;
		flapIndexNE = flapIndexNE == 1 ? 2 : 5;
		flapIndexSW = flapIndexSW == 3 ? 4 : 7;
		flapIndexSE = flapIndexSE == 7 ? 8 : 5;
	}

	PopulateRiteColFlaps();
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

	if (constellation == FS_HORIZONTAL)
	{
		horzLeftRight = 0;
	}
	else if (constellation == FS_QUARTERED)
	{
		flapIndexNW = flapIndexNW == 1 ? 0 : 3;
		flapIndexNE = flapIndexNE == 2 ? 1 : 4;
		flapIndexSW = flapIndexSW == 4 ? 3 : 6;
		flapIndexSE = flapIndexSE == 5 ? 4 : 7;
	}

	PopulateLeftColFlaps();
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

SDL_Texture* _NewTexture(SDL_Renderer* renderer, bool transparentAble)
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
