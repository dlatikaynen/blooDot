#pragma once
#include "gamestate.h"

// Players have personality and character.
// So it makes sense to refer to them by named const sometimes
constexpr int const iP1 = 0;
constexpr int const iP2 = 1;
constexpr int const iP3 = 2;
constexpr int const iP4 = 3;

typedef struct InventoryItemStruct
{
	Ding Ding;
	int Quantity;
	int Quality;
} InventoryItem;

typedef struct MobStateStruct
{
	unsigned short Vitals;
	PointInWorld Position;
	SDL_Point Offset;
	int Orientation;
	std::list<InventoryItem> Inventory;
} MobState;

namespace blooDot::Player
{
	extern int NumPlayers;
}