#pragma once
#include <SDL.h>
#include <cairo.h>
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
	RooofMesh,
	Player1,
	Player2,
	Player3,
	Player4
};

DingProps GetDingDefaultProps(const Ding ding);
bool IsOwnerDrawnDing(const Ding ding);
int GetDingResourceKey(const Ding ding);
char const* GetDingName(const Ding ding);
void DrawDing(const Ding ding, cairo_t* canvas);