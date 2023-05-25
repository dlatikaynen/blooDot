#include "pch.h"
#include "xlations.h"
#include "gamestate.h"
#include "geom2d.h"

/* (0,0) is world center.
 * world data is loaded on demand, in 9 quadrants at a time
 * the data structure is a two-dimensional linked and stacked matrix */

std::array<std::shared_ptr<WorldSheet>, 9> worldSheets = { 0 };
std::vector<WorldRegion> worldRegions;

void ClearWorldData()
{
	for (auto i = 0; i < worldSheets.size(); ++i)
	{
		ClearWorldSheet(i);
	}

	worldRegions.clear();
}

void ClearWorldSheet(int sheetIndex)
{
	assert(sheetIndex >= 0 && sheetIndex < 9);
	if (worldSheets[sheetIndex])
	{
		worldSheets[sheetIndex].reset();
	}

	worldSheets[sheetIndex] = std::make_shared<WorldSheet>();
}

std::shared_ptr<WorldSheet> GetWorldSheet(int sheetX, int sheetY)
{
	const auto sheetIndex = (sheetY + 1) * 3 + (sheetX + 1);
	assert(sheetIndex >= 0 && sheetIndex < 9);

	return worldSheets[sheetIndex];
}

void AddRegion(WorldRegion regionDescriptor)
{
	regionDescriptor.bounds = GetPolyBoundingBox(&regionDescriptor.polygon);
	worldRegions.push_back(regionDescriptor);
}

WorldPieces* GetPiecesRelative(int worldX, int worldY)
{
	const auto sheet = GetWorldSheet(0, 0);
	const auto pieceIndex = ((worldY + WORLD_SHEET_CENTERPOINT) * WORLD_SHEET_SIDELENGTH + worldX + WORLD_SHEET_CENTERPOINT);
	
	return &sheet->arena[pieceIndex];
}

const char* GetRegionName(int worldX, int worldY)
{
	/* 1. intersect with the bounding boxes. first wins
	 *    if it has a fitting polygon, so overlapping regions
	 *    would be matched in the order of their adding to the world */
	for (const auto& region : worldRegions)
	{
		if (PointInRect(worldX, worldY, region.bounds))
		{
			/* 2. now intersect with the polygon */
			if (WorldCoordinateInRegion(&region.polygon, worldX, worldY))
			{
				return region.RegionName.c_str();
			}
		}
	}

	return literalregionNameUnknown;
}