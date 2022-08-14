#pragma once
#include <SDL.h>
#include "enums.h"

constexpr auto GRIDUNIT = 49;

enum Ding : unsigned short
{
	None,
	FloorSlate,
	WallClassic,
	BarrelIndigo
};

DingProps GetDingDefaultProps(Ding ding);