#pragma once

#include "..\PreCompiledHeaders.h"

/// Design-time and runtime state primitives for tiles/blocks in blooDot
enum ObjectBehaviors : UINT32
{
	Boring = 0,		// block with no properties
	Solid = 1 << 0,	// block where you can bang your head against
	Immersible = 1 << 1,  // block where mobs can walk inside (liquids, gases, loose solids)
	SluiceNS = 1 << 2,  // oneway 270° 
	SluiceWE = 1 << 3,  // oneway 0°
	SluiceEW = 1 << 4,  // oneway 180°
	SluiceSN = 1 << 5,  // oneway 90°
	Takeable = 1 << 6,  // goes to inventory when touched
	Pushable = 1 << 7,  // can be pushed
	Pullable = 1 << 8,  // can be pulled (with a rope or whatever)
	Shootable = 1 << 9,  // goes dead or to inventory when hit with a projectile object
	Openable = 1 << 10,  // has a lid that opens
	Closeable = 1 << 11,  // has an open lid that can be closed
	CanTakeStuffOut = 1 << 12,  // container able to contain, or currently containing stuff
	CanPutStuffIn = 1 << 13,  // container where (additinal) items can be put
	Slippery = 1 << 14,  // ice-like floor tiles
	CreatesDrag = 1 << 15,  // sand-like floor tiles
	CanSinkInto = 1 << 16,  // quicksand-like floor tiles
	Bouncy = 1 << 17,  // rubber-like obstacles
	Lethal = 1 << 18,   // kills when touched
	ActiveMoving = 1 << 19, // moves about on its own
	ImpairsVisibility = 1 << 20 // player can hide inside
};

constexpr inline ObjectBehaviors operator |(const ObjectBehaviors left, const ObjectBehaviors& right)
{
	return static_cast<ObjectBehaviors>(static_cast<int>(left) | static_cast<int>(right));
}

ObjectBehaviors& operator |=(ObjectBehaviors& a, ObjectBehaviors b);