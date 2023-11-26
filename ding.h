#pragma once
#include <SDL2/SDL.h>
#include <cairo/cairo.h>
#include <iostream>

#include "enums.h"
#include "dexassy.h"
#include "box2d/box2d.h"

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
	Player4,
	Hourflower,
	Hourflour,
	Hourpower,
	SnaviorMonumentHead,
	SnaviorMonumentFloor
};

typedef char const BloodotMarkup;

namespace blooDot::Dings
{
	bool InitializeDings();
	DingProps GetDingDefaultProps(const Ding ding);
	bool IsOwnerDrawnDing(const Ding ding);
	int GetDingResourceKey(const Ding ding);
	int GetDingDescriptionsResourceKey(const Ding ding);
	char const* GetDingDescriptionIndexEntry(const Ding ding);
	char const* GetDingName(const Ding ding);
	BloodotMarkup* GetDingDescription(const Ding ding);
	void DrawDing(const Ding ding, cairo_t* canvas);
	bool HasNontrivialCollisionPoly(const ::Ding ding);
	std::vector<b2PolygonShape> GetNontrivialCollisionPoly(const ::Ding ding);
	void TeardownDings();

	bool _LoadAndRegisterNonTrivialCollision(int chunkKey, std::vector<b2PolygonShape>* storeIn);
}
