#include "pch.h"
#include "ding.h"

DingProps GetDingDefaultProps(Ding ding)
{
	switch (ding)
	{
	case Ding::BarrelIndigo:
	case Ding::WallClassic:
		return DingProps::Walls;

	default:
		return DingProps::Floor;
	}
}