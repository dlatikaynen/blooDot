#pragma once
#include <SDL.h>
#include "enums.h"
#include "dexassy.h"

constexpr auto GRIDUNIT = 49;

enum Ding : unsigned short
{
	None,
	FloorSlate,
	FloorOchre,
	FloorPurple,
	FloorRock,
	FloorRockCracked,
	WallClassic,
	BarrelIndigo,
	BarrelWood,
	BarrelLoaded,
	Lettuce,
	Grass,
	Schaed,
	Snow,
	ChelF,
	RooofMesh
};

DingProps GetDingDefaultProps(Ding ding);
int GetDingResourceKey(Ding ding);
char const* GetDingName(Ding ding);