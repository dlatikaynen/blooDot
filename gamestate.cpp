#include "pch.h"
#include "gamestate.h"
#include "geom2d.h"

/* (0,0) is world center.
 * world data is loaded on demand, in 9 quadrants at a time
 * the data structure is a two-dimensional linked and stacked matrix */

std::array<WorldSheet*, 9> worldSheets;
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
		SDL_free(worldSheets[sheetIndex]);
	}

	worldSheets[sheetIndex] = NULL;
}

void ReplaceWorldSheet(int sheetX, int sheetY, WorldSheet* content)
{
	const auto sheetIndex = (sheetY + 1) * 3 + (sheetX + 1);
	assert(sheetIndex >= 0 && sheetIndex < 9);
	if (worldSheets[sheetIndex])
	{
		SDL_free(worldSheets[sheetIndex]);
	}

	worldSheets[sheetIndex] = content;
}

void AddRegion(WorldRegion regionDescriptor)
{
	regionDescriptor.bounds = GetPolyBoundingBox(&regionDescriptor.polygon);
	worldRegions.push_back(regionDescriptor);
}

WorldPiece GetPieceRelative(int worldX, int worldY)
{
	const auto sheet = worldSheets[static_cast<std::array<WorldSheet*, 9Ui64>::size_type>(1 * 3) + 1];
	const auto pieceIndex = ((worldY + WORLD_SHEET_CENTERPOINT) * WORLD_SHEET_SIDELENGTH + worldX + WORLD_SHEET_CENTERPOINT);
	
	return sheet->arena[pieceIndex];
}