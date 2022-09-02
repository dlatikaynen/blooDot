#include "pch.h"
#include "ding.h"
#include "xlations.h"

DingProps GetDingDefaultProps(const Ding ding)
{
	switch (ding)
	{
	case Ding::BarrelIndigo:
	case Ding::BarrelWood:
	case Ding::BarrelLoaded:
	case Ding::WallClassic:
		return DingProps::Walls;

	case Ding::Lettuce:
	case Ding::ChelF:
		return DingProps::Loot | DingProps::Walls;

	case Ding::Schaed:
		return DingProps::Mob | DingProps::Walls;

	case Ding::RooofMesh:
		return DingProps::Rooof;

	default:
		return DingProps::Floor;
	}
}

bool IsOwnerDrawnDing(const Ding ding)
{
	return ding == Ding::WallClassic;
}

int GetDingResourceKey(const Ding ding)
{
	switch (ding)
	{
	case Ding::BarrelIndigo:
		return CHUNK_KEY_DINGS_BARREL_INDIGO;

	case Ding::BarrelWood:
		return CHUNK_KEY_DINGS_BARREL_WOODEN;

	case Ding::BarrelLoaded:
		return CHUNK_KEY_DINGS_BARREL_LOADED;

	case Ding::FloorSlate:
		return CHUNK_KEY_DINGS_FLOORSTONETILE_SLATE;

	case Ding::FloorPurple:
		return CHUNK_KEY_DINGS_FLOORSTONETILE_PURPLE;

	case Ding::FloorOchre:
		return CHUNK_KEY_DINGS_FLOORSTONETILE_OCHRE;

	case Ding::FloorRock:
		return CHUNK_KEY_DINGS_FLOORROCKTILE;

	case Ding::FloorRockCracked:
		return CHUNK_KEY_DINGS_FLOORROCKTILE_CRACKED;

	case Ding::Lettuce:
		return CHUNK_KEY_DINGS_LETTUCE;

	case Ding::Schaed:
		return CHUNK_KEY_DINGS_SCHAED;

	case Ding::Snow:
		return CHUNK_KEY_DINGS_SNOW;

	case Ding::ChelF:
		return CHUNK_KEY_DINGS_CHEL_F;

	case Ding::Grass:
		return CHUNK_KEY_DINGS_GRASS;

	case Ding::RooofMesh:
		return CHUNK_KEY_DINGS_ROOOFTILE_MESH;

	default:
		assert(ding != ding);
		return -1;
	}
}

char const* GetDingName(const Ding ding)
{
	switch (ding)
	{
	case Ding::WallClassic:
		return dingNameWallClassic;

	case Ding::BarrelIndigo:
		return dingNameBarrelIndigo;

	case Ding::BarrelWood:
		return dingNameBarrelWood;

	case Ding::BarrelLoaded:
		return dingNameBarrelLoaded;

	case Ding::FloorSlate:
		return dingNameFloorSlate;

	case Ding::FloorPurple:
		return dingNameFloorPurple;

	case Ding::FloorOchre:
		return dingNameFloorOchre;

	case Ding::FloorRock:
		return dingNameFloorRock;

	case Ding::FloorRockCracked:
		return dingNameFloorRockCracked;

	case Ding::Lettuce:
		return dingNameLettuce;

	case Ding::Schaed:
		return dingNameSchaed;

	case Ding::Snow:
		return dingNameSnow;

	case Ding::ChelF:
		return dingNameChelF;

	case Ding::Grass:
		return dingNameGrass;

	case Ding::RooofMesh:
		return dingNameRooofMesh;

	default:
		assert(ding != ding);
		return nullptr;
	}
}

void DrawDing(const Ding ding, cairo_t* canvas)
{
	switch (ding)
	{
	case Ding::WallClassic:
		cairo_set_source_rgb(canvas, .3, .3, .3);
		cairo_rectangle(canvas, 0, 0, GRIDUNIT, GRIDUNIT);
		cairo_fill(canvas);
		cairo_set_source_rgb(canvas, .7, .7, .7);
		cairo_set_line_width(canvas, 3.);
		cairo_rectangle(canvas, 6, 6, GRIDUNIT - 12, GRIDUNIT - 12);
		cairo_stroke(canvas);
		break;

	default:
		assert(!"Don't know how to draw");
	}
}