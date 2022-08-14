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

	_Put(centerSheet, 1, 1, Ding::BarrelIndigo);
	_Put(centerSheet, -1, 1, Ding::BarrelIndigo);
	_Put(centerSheet, 1, -1, Ding::BarrelIndigo);
	_Put(centerSheet, -1, -1, Ding::BarrelIndigo);

	ReplaceWorldSheet(0, 0, centerSheet);

	return true;
}

void _Put(WorldSheet* sheet, int x, int y, Ding ding)
{
	DingInstance instance;
	instance.ding = ding;
	instance.props = DingProps::Floor;
	sheet->arena[static_cast<std::array<WorldPiece, 422Ui64>::size_type>(y + WORLD_SHEET_CENTERPOINT) * WORLD_SHEET_SIDELENGTH + x + WORLD_SHEET_CENTERPOINT]
		.dings
		.push_back(instance);
}