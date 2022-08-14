#include "pch.h"
#include "ding.h"

DingProps GetDingDefaultProps(Ding ding)
{
	switch (ding)
	{
	case Ding::BarrelIndigo:
	case Ding::WallClassic:
	case Ding::Lettuce:
		return DingProps::Walls;

	default:
		return DingProps::Floor;
	}
}

int GetDingResourceKey(Ding ding)
{
	switch (ding)
	{
	case Ding::BarrelIndigo:
		return CHUNK_KEY_DINGS_BARREL_INDIGO;

	case Ding::FloorSlate:
		return CHUNK_KEY_DINGS_FLOORSTONETILE_SLATE;

	case Ding::Lettuce:
		return CHUNK_KEY_DINGS_LETTUCE;

	default:
		return -1;
	}
}
