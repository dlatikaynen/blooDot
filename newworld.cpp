#include "pch.h"
#include "newworld.h"

bool InitializeNewWorld()
{
	ClearWorldData();

	WorldRegion scullery;
	scullery.belongsTo = NULL;
	scullery.RegionId = 1;
	scullery.RegionName = std::string(regionNameScullery);
	scullery.polygon.push_back({ -3,-3 });
	scullery.polygon.push_back({ 3,-3 });
	scullery.polygon.push_back({ 3,3 });
	scullery.polygon.push_back({ -3,3 });
	AddRegion(scullery);

	const auto centerSheet = (WorldSheet *)SDL_malloc(sizeof(WorldSheet));
	if (!centerSheet)
	{
		const auto allocError = SDL_GetError();
		ReportError("Failed to allocate center sheet", allocError);
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


	ReplaceWorldSheet(0, 0, centerSheet);

	return true;
}

void _Put(WorldSheet* sheet, int x, int y, Ding ding, DingProps props)
{
	DingInstance instance{};
	instance.ding = ding;
	instance.props = props == DingProps::Default ? GetDingDefaultProps(ding) : props;
	const auto pieceIndex = (y + WORLD_SHEET_CENTERPOINT) * WORLD_SHEET_SIDELENGTH + x + WORLD_SHEET_CENTERPOINT;
	sheet->arena[pieceIndex]
		.dings
		.push_back(instance);
}