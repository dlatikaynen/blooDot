#include "pch.h"
#include "renderstate.h"

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
│3        │4     ┃  │ VP  ┃-> │
│         │      ┗━━│━━━━━┛   │
├─────────┼─────────┼─────────┤
│6        │7        │8        │
│         │         │         │
└─────────┴─────────┴─────────┘

* VP is viewport.
*/

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