#pragma once
#include "gamestate.h"

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