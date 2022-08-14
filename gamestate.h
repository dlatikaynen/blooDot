#pragma once
#include <SDL.h>
#include "ding.h"
#include "enums.h"

constexpr auto WORLD_SHEET_SIDELENGTH = 120;

typedef struct DingInstance {
	Ding ding;
	DingProps props;
} DingInstance;

/// <summary>
/// 
/// </summary>
typedef struct WorldPeace
{
	std::vector<DingInstance> dings;
} WorldPiece;

typedef struct WorldSheet
{
	// how many sheets from the center sheet
	int originDx;
	int originDy;
	std::array<WorldPiece, WORLD_SHEET_SIDELENGTH * WORLD_SHEET_SIDELENGTH> arena;
} WorldSheet;

typedef struct PointInWorld
{
	int x;
	int y;
} PointInWorld;

/// <summary>
/// A region can span world sheets.
/// Disjunct parts of regions point to their main region
/// for naming and identification
/// </summary>
typedef struct WorldRegion
{
	int RegionId;
	std::string RegionName;
	std::vector<PointInWorld> polygon;
	WorldRegion* belongsTo = NULL;
} WorldRegion;

void ClearWorldData();
void ClearWorldSheet(int sheetIndex);
void AddRegion(WorldRegion regionDescriptor);
void ReplaceWorldSheet(int sheetX, int sheetY, WorldSheet* content);