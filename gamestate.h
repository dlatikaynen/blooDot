#pragma once
#include <SDL.h>
#include "ding.h"
#include "enums.h"

constexpr auto WORLD_SHEET_SIDELENGTH = 120;
constexpr auto WORLD_SHEET_CENTERPOINT = WORLD_SHEET_SIDELENGTH / 2;

typedef struct DingInstance {
	Ding ding;
	DingProps props;
} DingInstance;

/// <summary>
/// One grid unit full of information.
/// Contains data for all three layers
/// Multi-cell objects are repeated as
/// pointers in every neighboring cell
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
	SDL_Rect bounds;
	WorldRegion* belongsTo = NULL;
} WorldRegion;

void ClearWorldData();
void ClearWorldSheet(int sheetIndex);
void AddRegion(WorldRegion regionDescriptor);
void ReplaceWorldSheet(int sheetX, int sheetY, WorldSheet* content);
WorldPiece GetPieceRelative(int worldX, int worldY);
const char* GetRegionName(int worldX, int worldY);