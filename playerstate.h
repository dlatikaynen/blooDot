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

typedef enum VitalSlotDedicationEnum
{
	VSL_Phys = 0,
	VSL_Heat = 1,
	VSL_Cold = 2,
	VSL_Chem = 3
} VitalSlotDedication;

typedef struct MobStateStruct
{
	Ding WhoIsIt;

	// bits 0..1 --> NE petal, 2..3 --> NW petal, 4..5 --> SW petal, 6..7 --> SE petal
	Uint8 SlotDedication; 

	// bits 0..2 --> phys, 3..5 --> heat, 6..8 --> cold, 9..11 --> chem
	Uint16 Capacity;

	// bits 0..3 --> phys, 4..7 --> heat, 8..11 --> cold, 12..15 --> chem
	Uint16 Vitals; // conveniency property for gameplay. value is redundant via petals x dedication
	
	// bits 0..2 --> NE petal, 3..5 --> NW petal, 6..8 --> SW petal, 9..11 --> SE petal
	Uint16 Petals;

	PointInWorld Position;
	SDL_Point Offset;
	int Orientation;
	std::list<InventoryItem> Inventory;
} MobState;

namespace blooDot::Player
{
	extern int NumPlayers;

	void InitializePlayers(int numPlayers);
	MobState* GetState(int ofPlayerIndex);
	void VitalIncrease(MobState* mobState, VitalSlotDedication dedication);
	void VitalDecrease(MobState* mobState, VitalSlotDedication dedication);
}