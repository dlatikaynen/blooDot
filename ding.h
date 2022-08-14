#pragma once
#include <SDL.h>
#include "enums.h"
#include "dexassy.h"

constexpr auto GRIDUNIT = 49;

enum Ding : unsigned short
{
	None,
	FloorSlate,
	WallClassic,
	BarrelIndigo,
	Lettuce
};

DingProps GetDingDefaultProps(Ding ding);
int GetDingResourceKey(Ding ding);