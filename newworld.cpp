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

	for (auto y = -3; y <= 3; ++y)
	{
		for (auto x = -4; x <= 4; ++x)
		{
			_Put(centerSheet, x, y, Ding::FloorSlate);
		}
	}

	_Put(centerSheet, 1, 1, Ding::BarrelIndigo);
	_Put(centerSheet, -1, 1, Ding::BarrelIndigo);
	_Put(centerSheet, 1, -1, Ding::BarrelIndigo);
	_Put(centerSheet, -1, -1, Ding::BarrelIndigo);
	_Put(centerSheet, -2, 0, Ding::Lettuce);

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