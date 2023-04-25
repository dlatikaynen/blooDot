#include "pch.h"
#include "renderstate.h"
#include "playerstate.h"
#include "ding.h"
#include "xlations.h"

extern SDL_Renderer* GameViewRenderer;

/* this is where the centerpoint of the nine flaps is in the world */
extern int originDx;
extern int originDy;

/* for screens where we render only a part (square) */
extern int viewportOffsetX;
extern int viewportOffsetY;

/* and this is to decide when we need a flapover in either direction */
int flapLocalDx = 0;
int flapLocalDy = 0;

int flapW = 0;
int flapH = 0;
int halfW = 0;
int halfH = 0;
int flapoverTresholdX = 0;
int flapoverTresholdY = 0;

SDL_Texture* flapsFloor[9];
SDL_Texture* flapsWalls[9];
SDL_Texture* flapsRooof[9];

MobState* player1 = NULL;
MobState* player2 = NULL;
MobState* player3 = NULL;
MobState* player4 = NULL;

DingLocator* sprite1 = NULL;
DingLocator* sprite2 = NULL;
DingLocator* sprite3 = NULL;
DingLocator* sprite4 = NULL;

#ifndef NDEBUG
SDL_Texture* debugFlap;
#endif

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
SDL_Rect bungDst;
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
	flapoverTresholdX = width / 3 * 2;
	flapH = height;
	halfH = height / 2;
	flapoverTresholdY = height / 3 * 2;

	/* the centerpiece is constant wrt the viewport */
	bunghole = SDL_Rect{ 0,0,width,height };
	bungDst = SDL_Rect{ viewportOffsetX,viewportOffsetY,width,height };

	for (auto i = 0; i < 9; ++i)
	{
		flapsFloor[i] = NewTexture(GameViewRenderer, flapW, flapH, false);
		if (!flapsFloor[i]) 
		{
			return false;
		}

		flapsWalls[i] = NewTexture(GameViewRenderer, flapW, flapH, true);
		if (!flapsWalls[i])
		{
			return false;
		}

		flapsRooof[i] = NewTexture(GameViewRenderer, flapW, flapH, true);
		if (!flapsRooof[i])
		{
			return false;
		}
	}

	RecomputeFlapConstellation();
#ifndef NDEBUG
	debugFlap = NewTexture(GameViewRenderer, flapW, flapH, true, true);
#endif

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
		std::cout << literaldialogPlayer3Perennial << "\n";
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

/// <summary>
/// Used to be the most mysterious thing in retro
/// Nowadays, there is no pel pan, no interrupts,
/// just a massive waste of stupidly moving 8888s
/// So we're here to making it complicated again.
/// </summary>
void Scroll(const int dx, const int dy)
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
			_SnapHorizontal(dx);
		}
		else if (isVert)
		{
			/* break out into a horizontally locked (vertical-only) slide */
			_SnapVertical(dy);
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

			_Unsnap(dx, dy);
		}

		break;

	case FS_HORIZONTAL:
		if (isHorz)
		{
			if (isVertSnapped) [[unlikely]]
			{
#ifndef NDEBUG
				std::cout << "HORZ UP THE BUNGHOLE\n";
#endif

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
				_SnapVertical(dy);
			}
			else
			{
				if (horzLeftRight == 1)
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
				else
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

				_Unsnap(dx + horzLeftSrc.x, dy);
			}
		}

		break;

	case FS_VERTICAL:
		if (isVert)
		{
			if (isHorzSnapped) [[unlikely]]
			{
#ifndef NDEBUG
				std::cout << "VERT UP THE BUNGHOLE\n";
#endif

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
				_SnapHorizontal(dx);
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
						/* SE */
						flapIndexNW = 4; flapIndexNE = 5; flapIndexSW = 7; flapIndexSE = 8;
					}
					else
					{
						/* NE */
						flapIndexNW = 1; flapIndexNE = 2; flapIndexSW = 4; flapIndexSE = 5;
					}
				}

				_Unsnap(dx, dy + vertUpperSrc.y);
			}
		}

		break;

	default:
		assert(constellation == FS_QUARTERED);

		if (isVertSnapped && isHorzSnapped) [[unlikely]]
		{
			/* jackpot */
#ifndef NDEBUG
			std::cout << "RIGHT IN THE BUNGHOLE\n";
#endif

			constellation = FS_BUNGHOLE;
		}
		else if (isHorzSnapped)
		{
			/* freerange to horizontally gliding */
			horzLeftRight = flapIndexNW == 0 || flapIndexNW == 3 ? 0 : 1;
			horzLeftSrc = { quadrantNWSrc.x + dx,0,quadrantNWSrc.w - dx,flapH };
			horzLeftDst = { quadrantNWDst.x,viewportOffsetY,quadrantNWDst.w - dx,flapH };
			horzRiteSrc = { quadrantNESrc.x,0,quadrantNESrc.w + dx,flapH };
			horzRiteDst = { quadrantNEDst.x - dx,viewportOffsetY,quadrantNEDst.w + dx,flapH };

#ifndef NDEBUG
			std::cout << "RE-SNAPPED HORZ\n";
#endif

			constellation = FS_HORIZONTAL;
		}
		else if (isVertSnapped)
		{
			/* freerange to vertically gliding */
			vertUpperLower = flapIndexNW == 0 || flapIndexNW == 1 ? 0 : 1;
			vertUpperSrc = { 0,quadrantNWSrc.y + dy,flapW,quadrantNWSrc.h - dy };
			vertUpperDst = { viewportOffsetX,quadrantNWDst.y,flapW,quadrantNWDst.h - dy };
			vertLowerSrc = { 0,quadrantSWSrc.y,flapW,quadrantSWSrc.h + dy };
			vertLowerDst = { viewportOffsetX,quadrantSWDst.y - dy,flapW,quadrantSWDst.h + dy };

#ifndef NDEBUG
			std::cout << "RE-SNAPPED VERT\n";
#endif

			constellation = FS_VERTICAL;
		}
		else [[likely]]
		{
			/* remains freeform */
			quadrantNWSrc.x += dx;
			quadrantNWSrc.w -= dx;
			quadrantNWSrc.y += dy;
			quadrantNWSrc.h -= dy;
			quadrantNWDst.w -= dx;
			quadrantNWDst.h -= dy;

			quadrantNESrc.w += dx;
			quadrantNESrc.y += dy;
			quadrantNESrc.h -= dy;
			quadrantNEDst.x -= dx;
			quadrantNEDst.w += dx;
			quadrantNEDst.h -= dy;

			quadrantSWSrc.x += dx;
			quadrantSWSrc.w -= dx;
			quadrantSWSrc.h += dy;
			quadrantSWDst.w -= dx;
			quadrantSWDst.y -= dy;
			quadrantSWDst.h += dy;

			quadrantSESrc.w += dx;
			quadrantSESrc.h += dy;
			quadrantSEDst.x -= dx;
			quadrantSEDst.y -= dy;
			quadrantSEDst.w += dx;
			quadrantSEDst.h += dy;
		}

		break;
	}

	/* flapovers are looked at after the fact */
	flapLocalDx += dx;
	flapLocalDy += dy;
	if (abs(flapLocalDx) > flapoverTresholdX)
	{
		if (flapLocalDx > 0)
		{
			_FlapoverRite();
			flapLocalDx -= flapW;
		}
		else
		{
			_FlapoverLeft();
			flapLocalDx += flapW;
		}
	}
	else if (abs(flapLocalDy) > flapoverTresholdY)
	{
		/* this is an else if because we don't want to do
		 * two flapovers at once if we're hitting a diagonal exactly. */
		if (flapLocalDy > 0)
		{
			_FlapoverDown();
			flapLocalDy -= flapH;
		}
		else
		{
			_FlapoverUuup();
			flapLocalDy += flapH;
		}
	}
}

void PopulateAllFlaps()
{
	for (unsigned short i = 0; i < 9; ++i)
	{
		flapIndirection[i] = i;
		PopulateFlap(i, flapWorldOffsetsX[i], flapWorldOffsetsY[i]);
	}
}

void PopulateTopRowFlaps(const int worldRow)
{
	const auto medianCol = GetCurrentBungCol();

	PopulateFlap(0, medianCol - 1, worldRow);
	PopulateFlap(1, medianCol, worldRow);
	PopulateFlap(2, medianCol + 1, worldRow);
}

void PopulateBottomRowFlaps(const int worldRow)
{
	const auto medianCol = GetCurrentBungCol();

	PopulateFlap(6, medianCol - 1, worldRow);
	PopulateFlap(7, medianCol, worldRow);
	PopulateFlap(8, medianCol + 1, worldRow);
}

void PopulateLeftColFlaps(const int worldColumn)
{	
	const auto medianRow = GetCurrentBungRow();
	
	PopulateFlap(0, worldColumn, medianRow - 1);
	PopulateFlap(3, worldColumn, medianRow);
	PopulateFlap(6, worldColumn, medianRow + 1);
}

void PopulateRiteColFlaps(const int worldColumn)
{
	const auto medianRow = GetCurrentBungRow();

	PopulateFlap(2, worldColumn, medianRow - 1);
	PopulateFlap(5, worldColumn, medianRow);
	PopulateFlap(8, worldColumn, medianRow + 1);
}

/// <summary>
/// The top row becomes the bottom row,
/// the middle and bottom move to the top,
/// the new bottom row is scheduled for repopulation
/// </summary>
void _FlapoverDown()
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

#ifndef NDEBUG
	std::cout << "Flapped over downwards\n";
#endif
	
	const auto rowInWorld = FlapAwarenessDown();
	PopulateBottomRowFlaps(rowInWorld);
}

/// <summary>
/// The bottom row becomes the top row,
/// the middle and top move to the bottom,
/// the new top row is scheduled for repopulation
/// </summary>
void _FlapoverUuup()
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

#ifndef NDEBUG
	std::cout << "Flapped over upwards\n";
#endif

	const auto rowInWorld = FlapAwarenessUuup();
	PopulateTopRowFlaps(rowInWorld);
}

/// <summary>
/// The right column becomes the leftmost column,
/// the middle and left move on to the right,
/// the new leftmost column is scheduled for repopulation
/// </summary>
void _FlapoverLeft()
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

#ifndef NDEBUG
	std::cout << "Flapped over on the port side\n";
#endif

	const auto columnInWorld = FlapAwarenessLeft();
	PopulateLeftColFlaps(columnInWorld);
}

/// <summary>
/// The left column becomes the rightmost column,
/// the middle and right move one to the left,
/// the new rightmost column is scheduled for repopulation
/// (this is the scenario depicted in the drawing)
/// </summary>
void _FlapoverRite()
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

#ifndef NDEBUG
	std::cout << "Flapped over starbord\n";
#endif

	const auto columnInWorld = FlapAwarenessRite();
	PopulateRiteColFlaps(columnInWorld);
}

void RenderFloorBung()
{	
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndirection[BUNGHOLE]], &bunghole, &bungDst);
}

void RenderFloorVert()
{
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndirection[flapIndicesVert[vertUpperLower]]], &vertUpperSrc, &vertUpperDst);
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndirection[flapIndicesVert[vertUpperLower + 1]]], &vertLowerSrc, &vertLowerDst);
}

void RenderFloorHorz()
{
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndirection[flapIndicesHorz[horzLeftRight]]], &horzLeftSrc, &horzLeftDst);
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndirection[flapIndicesHorz[horzLeftRight + 1]]], &horzRiteSrc, &horzRiteDst);
}

void RenderFloorQuart()
{
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndirection[flapIndexNW]], &quadrantNWSrc, &quadrantNWDst);
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndirection[flapIndexNE]], &quadrantNESrc, &quadrantNEDst);
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndirection[flapIndexSW]], &quadrantSWSrc, &quadrantSWDst);
	SDL_RenderCopy(GameViewRenderer, flapsFloor[flapIndirection[flapIndexSE]], &quadrantSESrc, &quadrantSEDst);
}

void NudgePlayer(int playerIndex, int accelerationX, int accelerationY)
{
	switch (playerIndex)
	{
	case iP1:
		player1->Offset.x += accelerationX;
		player1->Offset.y += accelerationY;
		break;

	case iP2:
		player2->Offset.x += accelerationX;
		player2->Offset.y += accelerationY;
		break;

	case iP3:
		player3->Offset.x += accelerationX;
		player3->Offset.y += accelerationY;
		break;

	case iP4:
		player4->Offset.x += accelerationX;
		player4->Offset.y += accelerationY;
		break;

	}
}

void RenderMobs()
{
	if (player1 == NULL)
	{
		player1 = blooDot::Player::GetState(iP1);
	}

	if (sprite1 == NULL)
	{
		sprite1 = GetDing(player1->WhoIsIt);
	}

	if (blooDot::Player::NumPlayers >= 2)
	{
		if (player2 == NULL)
		{
			player2 = blooDot::Player::GetState(iP2);
		}

		if (blooDot::Player::NumPlayers >= 3)
		{
			if (player3 == NULL)
			{
				player3 = blooDot::Player::GetState(iP3);
			}

			if (player4 == NULL && blooDot::Player::NumPlayers == 4)
			{
				player4 = blooDot::Player::GetState(iP4);
			}
		}
	}

	const SDL_Rect dst1 = {
		player1->Offset.x,
		player1->Offset.y,
		49,
		49 
	};

	constexpr SDL_Point center = { 25,25 };

	SDL_RenderCopyEx(
		GameViewRenderer,
		sprite1->onSheet,
		&sprite1->src,
		&dst1,
		(const double)player1->Orientation,
		&center,
		SDL_RendererFlip::SDL_FLIP_VERTICAL
	);

	if(blooDot::Player::NumPlayers == 1 || player2 == NULL)
	{
		return;
	}

	if (sprite2 == NULL)
	{
		sprite2 = GetDing(player2->WhoIsIt);
	}

	const SDL_Rect dst2 = {
		player2->Offset.x,
		player2->Offset.y,
		49,
		49
	};

	SDL_RenderCopyEx(
		GameViewRenderer,
		sprite2->onSheet,
		&sprite2->src,
		&dst2,
		(const double)player2->Orientation,
		&center,
		SDL_RendererFlip::SDL_FLIP_VERTICAL
	);

	if (blooDot::Player::NumPlayers == 2 || player3 == NULL)
	{
		return;
	}

	if (sprite3 == NULL)
	{
		sprite3 = GetDing(player3->WhoIsIt);
	}

	const SDL_Rect dst3 = {
		player3->Offset.x,
		player3->Offset.y,
		49,
		49
	};

	SDL_RenderCopyEx(
		GameViewRenderer,
		sprite3->onSheet,
		&sprite3->src,
		&dst3,
		(const double)player3->Orientation,
		&center,
		SDL_RendererFlip::SDL_FLIP_VERTICAL
	);

	if (blooDot::Player::NumPlayers == 3 || player4 == NULL)
	{
		return;
	}

	if (sprite4 == NULL)
	{
		sprite4 = GetDing(player4->WhoIsIt);
	}

	const SDL_Rect dst4 = {
		player4->Offset.x,
		player4->Offset.y,
		49,
		49
	};

	SDL_RenderCopyEx(
		GameViewRenderer,
		sprite4->onSheet,
		&sprite4->src,
		&dst4,
		(const double)player4->Orientation,
		&center,
		SDL_RendererFlip::SDL_FLIP_VERTICAL
	);
}

void RenderWallsAndRooofBung()
{
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndirection[BUNGHOLE]], &bunghole, &bungDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndirection[BUNGHOLE]], &bunghole, &bungDst);
}

void RenderWallsAndRooofVert()
{
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndirection[flapIndicesVert[vertUpperLower]]], &vertUpperSrc, &vertUpperDst);
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndirection[flapIndicesVert[vertUpperLower + 1]]], &vertLowerSrc, &vertLowerDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndirection[flapIndicesVert[vertUpperLower + 1]]], &vertLowerSrc, &vertLowerDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndirection[flapIndicesVert[vertUpperLower]]], &vertUpperSrc, &vertUpperDst);
}

void RenderWallsAndRooofHorz()
{
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndirection[flapIndicesHorz[horzLeftRight]]], &horzLeftSrc, &horzLeftDst);
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndirection[flapIndicesHorz[horzLeftRight + 1]]], &horzRiteSrc, &horzRiteDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndirection[flapIndicesHorz[horzLeftRight]]], &horzLeftSrc, &horzLeftDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndirection[flapIndicesHorz[horzLeftRight + 1]]], &horzRiteSrc, &horzRiteDst);
}

void RenderWallsAndRooofQuart()
{
	assert(quadrantNWSrc.w + quadrantNESrc.w == flapW);
	assert(quadrantNWSrc.h == quadrantNESrc.h && quadrantNWSrc.h > 0);
	assert(quadrantNWSrc.y == quadrantNESrc.y && quadrantNWSrc.y > 0);

	assert(quadrantSWSrc.w + quadrantSESrc.w == flapW);
	assert(quadrantSWSrc.h == quadrantSESrc.h && quadrantSWSrc.h > 0);
	assert(quadrantSWSrc.y == quadrantSESrc.y && quadrantSWSrc.y == 0);

	assert(quadrantNWSrc.h + quadrantSWSrc.h == flapH);
	assert(quadrantNESrc.h + quadrantSESrc.h == flapH);

	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndirection[flapIndexNW]], &quadrantNWSrc, &quadrantNWDst);
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndirection[flapIndexNE]], &quadrantNESrc, &quadrantNEDst);
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndirection[flapIndexSW]], &quadrantSWSrc, &quadrantSWDst);
	SDL_RenderCopy(GameViewRenderer, flapsWalls[flapIndirection[flapIndexSE]], &quadrantSESrc, &quadrantSEDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndirection[flapIndexNW]], &quadrantNWSrc, &quadrantNWDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndirection[flapIndexNE]], &quadrantNESrc, &quadrantNEDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndirection[flapIndexSW]], &quadrantSWSrc, &quadrantSWDst);
	SDL_RenderCopy(GameViewRenderer, flapsRooof[flapIndirection[flapIndexSE]], &quadrantSESrc, &quadrantSEDst);
}

void RenderstateTeardown()
{
#ifndef NDEBUG
	SDL_DestroyTexture(debugFlap);
#endif

	for (auto i = 0; i < 9; ++i)
	{
		SDL_DestroyTexture(flapsFloor[i]);
		SDL_DestroyTexture(flapsWalls[i]);
		SDL_DestroyTexture(flapsRooof[i]);
	}
}

void _SnapHorizontal(int dx)
{
	if (dx > 0)
	{
		horzLeftRight = 1;
		horzLeftSrc = { dx,0,flapW - dx,flapH };
		horzLeftDst = { viewportOffsetX,viewportOffsetY,flapW - dx,flapH };
		horzRiteSrc = { 0,0,dx,flapH };
		horzRiteDst = { viewportOffsetX + (flapW - dx),viewportOffsetY,dx,flapH };
	}
	else
	{
		horzLeftRight = 0;
		horzLeftSrc = { flapW + dx,0,-dx,flapH };
		horzLeftDst = { viewportOffsetX,viewportOffsetY,-dx,flapH };
		horzRiteSrc = { 0,0,flapW + dx,flapH };
		horzRiteDst = { viewportOffsetX - dx,viewportOffsetY,flapW + dx,flapH };
	}

#ifndef NDEBUG
	std::cout << "HORZSNAPPED\n";
#endif

	constellation = FS_HORIZONTAL;
}

void _SnapVertical(int dy)
{
	if (dy > 0)
	{
		vertUpperLower = 1;
		vertUpperSrc = { 0,dy,flapW,flapH - dy };
		vertUpperDst = { viewportOffsetX,viewportOffsetY,flapW,flapH - dy };
		vertLowerSrc = { 0,0,flapW,dy };
		vertLowerDst = { viewportOffsetX,viewportOffsetY + (flapH - dy),flapW,dy };
	}
	else
	{
		vertUpperLower = 0;
		vertUpperSrc = { 0,flapH + dy,flapW,-dy };
		vertUpperDst = { viewportOffsetX,viewportOffsetY,flapW,-dy };
		vertLowerSrc = { 0,0,flapW,flapH + dy };
		vertLowerDst = { viewportOffsetX,viewportOffsetY - dy,flapW,flapH + dy };
	}

#ifndef NDEBUG
	std::cout << "VERTSNAPPED\n";
#endif

	constellation = FS_VERTICAL;
}

void _Unsnap(int dx, int dy)
{
	if (dx > 0)
	{
		quadrantNWSrc.x = dx;
		quadrantSWSrc.x = dx;
		quadrantNWSrc.w = flapW - dx;
		quadrantSWSrc.w = flapW - dx;
		quadrantNWDst.x = viewportOffsetX;
		quadrantSWDst.x = viewportOffsetX;
		quadrantNWDst.w = flapW - dx;
		quadrantSWDst.w = flapW - dx;

		quadrantNESrc.x = 0;
		quadrantSESrc.x = 0;
		quadrantNESrc.w = dx;
		quadrantSESrc.w = dx;
		quadrantNEDst.x = viewportOffsetX + (flapW - dx);
		quadrantSEDst.x = viewportOffsetX + (flapW - dx);
		quadrantNEDst.w = dx;
		quadrantSEDst.w = dx;
	}
	else
	{
		quadrantNWSrc.x = flapW + dx;
		quadrantSWSrc.x = flapW + dx;
		quadrantNWSrc.w = -dx;
		quadrantSWSrc.w = -dx;
		quadrantNWDst.x = viewportOffsetX;
		quadrantSWDst.x = viewportOffsetY;
		quadrantNWDst.w = -dx;
		quadrantSWDst.w = -dx;

		quadrantNESrc.x = 0;
		quadrantSESrc.x = 0;
		quadrantNESrc.w = flapW + dx;
		quadrantSESrc.w = flapW + dx;
		quadrantNEDst.x = viewportOffsetX - dx;
		quadrantSEDst.x = viewportOffsetX - dx;
		quadrantNEDst.w = flapW + dx;
		quadrantSEDst.w = flapW + dx;
	}

	if (dy > 0)
	{
		quadrantNWSrc.y = dy;
		quadrantNESrc.y = dy;
		quadrantNWSrc.h = flapH - dy;
		quadrantNESrc.h = flapH - dy;
		quadrantNWDst.y = viewportOffsetY;
		quadrantNEDst.y = viewportOffsetY;
		quadrantNWDst.h = flapH - dy;
		quadrantNEDst.h = flapH - dy;

		quadrantSWSrc.y = 0;
		quadrantSESrc.y = 0;
		quadrantSWSrc.h = dy;
		quadrantSESrc.h = dy;
		quadrantSWDst.y = viewportOffsetY + (flapH - dy);
		quadrantSEDst.y = viewportOffsetY + (flapH - dy);
		quadrantSWDst.h = dy;
		quadrantSEDst.h = dy;
	}
	else
	{
		quadrantNWSrc.y = flapH + dy;
		quadrantNESrc.y = flapH + dy;
		quadrantNWSrc.h = -dy;
		quadrantNESrc.h = -dy;
		quadrantNWDst.y = viewportOffsetY;
		quadrantNEDst.y = viewportOffsetY;
		quadrantNWDst.h = -dy;
		quadrantNEDst.h = -dy;

		quadrantSWSrc.y = 0;
		quadrantSESrc.y = 0;
		quadrantSWSrc.h = flapH + dy;
		quadrantSESrc.h = flapH + dy;
		quadrantSWDst.y = viewportOffsetY - dy;
		quadrantSEDst.y = viewportOffsetY - dy;
		quadrantSWDst.h = flapH + dy;
		quadrantSEDst.h = flapH + dy;
	}

#ifndef NDEBUG
	std::cout << "UNSNAPPED\n";
#endif

	constellation = FS_QUARTERED;
}