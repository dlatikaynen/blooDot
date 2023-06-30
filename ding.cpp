#include "pch.h"
#include "ding.h"
#include "xlations.h"
#include "polypartition.h"

DingProps GetDingDefaultProps(const Ding ding)
{
	switch (ding)
	{
	case Ding::Player1:
	case Ding::Player2:
	case Ding::Player3:
	case Ding::Player4:
	case Ding::BarrelIndigo:
	case Ding::BarrelWood:
	case Ding::BarrelLoaded:
		return DingProps::Mob | DingProps::Walls;

	case Ding::WallClassic:
		return DingProps::Walls;

	case Ding::Lettuce:
	case Ding::ChelF:
	case Ding::Hourflower:
	case Ding::Hourflour:
	case Ding::Hourpower:
		return DingProps::Loot | DingProps::Walls;

	case Ding::Schaed:
		return DingProps::Mob | DingProps::Walls;

	case Ding::RooofMesh:
		return DingProps::Rooof;

	case Ding::SnaviorMonumentHead:
		return DingProps::Rooof;

	case Ding::SnaviorMonumentFloor:
		return DingProps::Floor;

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
	case Ding::Player1:
		return CHUNK_KEY_MOBS_PLAYER1;

	case Ding::Player2:
		return CHUNK_KEY_MOBS_PLAYER2;

	case Ding::Player3:
		return CHUNK_KEY_MOBS_PLAYER3;

	case Ding::Player4:
		return CHUNK_KEY_MOBS_PLAYER4;

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

	case Ding::SnaviorMonumentHead:
		return CHUNK_KEY_DINGS_SNURCH_SNAVIOR_HEAD_ROOOF;

	case Ding::SnaviorMonumentFloor:
		return CHUNK_KEY_DINGS_SNURCH_SNAVIOR_FLOOR;

	default:
		assert(ding != ding);
		return -1;
	}
}

char const* GetDingName(const Ding ding)
{
	switch (ding)
	{
	case Ding::Player1:
		return literalplayerName1;

	case Ding::Player2:
		return literalplayerName2;

	case Ding::Player3:
		return literalplayerName3;

	case Ding::Player4:
		return literalplayerName4;

	case Ding::WallClassic:
		return literaldingNameWallClassic;

	case Ding::BarrelIndigo:
		return literaldingNameBarrelIndigo;

	case Ding::BarrelWood:
		return literaldingNameBarrelWood;

	case Ding::BarrelLoaded:
		return literaldingNameBarrelLoaded;

	case Ding::FloorSlate:
		return literaldingNameFloorSlate;

	case Ding::FloorPurple:
		return literaldingNameFloorPurple;

	case Ding::FloorOchre:
		return literaldingNameFloorOchre;

	case Ding::FloorRock:
		return literaldingNameFloorRock;

	case Ding::FloorRockCracked:
		return literaldingNameFloorRockCracked;

	case Ding::Lettuce:
		return literaldingNameLettuce;

	case Ding::Schaed:
		return literaldingNameSchaed;

	case Ding::Snow:
		return literaldingNameSnow;

	case Ding::ChelF:
		return literaldingNameChelF;

	case Ding::Grass:
		return literaldingNameGrass;

	case Ding::RooofMesh:
		return literaldingNameRooofMesh;

	case Ding::SnaviorMonumentFloor:
	case Ding::SnaviorMonumentHead:
		return "";

	default:
		assert(ding != ding);
		return nullptr;
	}
}

int GetDingDescriptionsResourceKey(const Ding)
{
	return CHUNK_KEY_DING_DESCRIPTIONS_1;
}

char const* GetDingDescriptionIndexEntry(const Ding _ding)
{
	return (char const *)_ding;
}

BloodotMarkup* GetDingDescription(const Ding _ding)
{
	return (BloodotMarkup *)_ding;
}

namespace blooDot::Dings
{
	std::vector<b2PolygonShape> snaviorCollision;

	bool InitializeDings()
	{
		const auto& slicer = new TPPLPartition();
		TPPLPoly poly;
		poly.Init(4);
		poly[0].x = 1;
		poly[0].y = 2;
		poly[1].x = 7;
		poly[1].y = 3;
		poly[2].x = 5;
		poly[2].y = 9;
		poly[3].x = -3;
		poly[3].y = 5;
		TPPLPolyList polyList;
		if (slicer->ConvexPartition_OPT(&poly, &polyList) == 0)
		{
			delete slicer;
			return false;
		}

		for (const auto& finalPoly : polyList)
		{
			const auto numPoints = finalPoly.GetNumPoints();
			std::vector<b2Vec2> collisionPoly;
			for (auto i = 0; i < numPoints; ++i)
			{
				const auto& point = finalPoly.GetPoint(i);
				collisionPoly.push_back(b2Vec2(static_cast<float>(point.x), static_cast<float>(point.y)));
			}

			const auto numVertices = static_cast<int>(collisionPoly.size());
			if (numVertices != 0)
			{
				b2PolygonShape sC;
				sC.Set(collisionPoly.data(), numVertices);
				snaviorCollision.push_back(sC);
			}
		}

		delete slicer;
		return true;
	}

	bool HasNontrivialCollisionPoly(const ::Ding ding)
	{
		switch (ding)
		{
		case SnaviorMonumentFloor:
			return true;
		}

		return false;
	}

	std::vector<b2PolygonShape> GetNontrivialCollisionPoly(const ::Ding ding)
	{
		switch (ding)
		{
		case SnaviorMonumentFloor:
			return snaviorCollision;

		default:
			assert(!"Not a ding with nontrivial collision poly");
		}

		return std::vector<b2PolygonShape>();
	}

	void TeardownDings()
	{
		snaviorCollision.clear();
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
		assert(!"Don't know how to draw/not an ownerdrawn ding");
	}
}