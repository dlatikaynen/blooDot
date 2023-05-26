#include "pch.h"
#include "newworld.h"
#include "xlations.h"
#include <iostream>

bool InitializeNewWorld()
{
	ClearWorldData();

	WorldRegion scullery;
	//scullery.belongsTo = NULL;
	scullery.RegionId = 1;
	scullery.RegionName = std::string(literalregionNameScullery);
	scullery.polygon.push_back({ -3,-3 });
	scullery.polygon.push_back({ 3,-3 });
	scullery.polygon.push_back({ 3,3 });
	scullery.polygon.push_back({ -3,3 });

	scullery.polygon.push_back({ -3,3 });
	scullery.polygon.push_back({ -3,3 });
	scullery.polygon.push_back({ -3,3 });
	scullery.polygon.push_back({ -3,3 });
	scullery.polygon.push_back({ -3,3 });
	scullery.polygon.push_back({ -3,3 });

	AddRegion(scullery);
	
	auto centerSheet = GetWorldSheet(0, 0);
	if (!centerSheet)
	{
		const auto allocError = SDL_GetError();
		ReportError("Encountered unallocated center sheet", allocError);
		return false;
	}

	_Put(centerSheet, -7, -5, Ding::BarrelIndigo);

	for (auto y = -3; y < 3; ++y)
	{
		for (auto x = -4; x < 4; ++x)
		{
			_Put(centerSheet, x, y, Ding::FloorSlate);
		}
	}

	_Put(centerSheet, 1, 1, Ding::BarrelIndigo);
	_Put(centerSheet, -1, 1, Ding::BarrelIndigo);
	_Put(centerSheet, 1, -1, Ding::BarrelWood);
	_Put(centerSheet, 0, 0, Ding::ChelF);
	_Put(centerSheet, -1, -1, Ding::BarrelLoaded);
	_Put(centerSheet, -1, -1, Ding::RooofMesh);
	_Put(centerSheet, 2, 1, Ding::Grass);
	_Put(centerSheet, -2, 0, Ding::Lettuce);
	_Put(centerSheet, 2, 0, Ding::Schaed);
	_Put(centerSheet, 0, -2, Ding::Snow);
	_Put(centerSheet, 0, -3, Ding::WallClassic);

	_Put(centerSheet, 6, 4, Ding::Grass);
	_Put(centerSheet, 7, 4, Ding::Grass);
	_Put(centerSheet, 6, 5, Ding::Grass);
	_Put(centerSheet, 7, 5, Ding::Grass);

	_Put(centerSheet, -10, 0, Ding::Lettuce);
	_Put(centerSheet, 8, 0, Ding::Lettuce);

	auto y = 2;
	for (auto i = 10; i < 120; ++i)
	{
		if (i % 5 == 0)
		{
			++y;
		}

		_Put(centerSheet, i, y, Ding::Lettuce);
	}

	y = 2;
	for (auto i = -10; i > -120; --i)
	{
		if (i % 5 == 0)
		{
			--y;
		}

		_Put(centerSheet, i, y, Ding::Schaed);
	}

	return true;
}

void _Put(std::shared_ptr<WorldSheet> sheet, int x, int y, Ding ding, DingProps props)
{
	const auto pieceIndex = (y + WORLD_SHEET_CENTERPOINT) * WORLD_SHEET_SIDELENGTH + x + WORLD_SHEET_CENTERPOINT;
	const auto instancePtr = std::make_shared<DingInstance>();

	/* 1. assign the properties */
	instancePtr->ding = ding;
	instancePtr->props = props == DingProps::Default ? GetDingDefaultProps(ding) : props;

	/* 2. register the entity in the world sheet storage.
	*     this step may become obsolete since they now live on the heap */
	sheet->stuff->push_back(instancePtr);

	/* 3. place its anchor point into the grid cell where it is anchored */
	sheet->arena[pieceIndex].push_back(instancePtr);

	/* 4. TODO: compute all other grid cells which it touches, so collision
	 *    detection via bounding squares will work */

}