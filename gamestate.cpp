#include "pch.h"
#include "gamestate.h"

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
	const auto sheetIndex = sheetY * 3 + sheetX;
	assert(sheetIndex >= 0 && sheetIndex < 9);
	if (worldSheets[sheetIndex])
	{
		SDL_free(worldSheets[sheetIndex]);
	}

	worldSheets[sheetIndex] = content;
}

void AddRegion(WorldRegion regionDescriptor)
{
	worldRegions.push_back(regionDescriptor);
}