#pragma once
#include <SDL.h>
#include "ding.h"
#include "enums.h"

constexpr auto WORLD_SHEET_SIDELENGTH = 120;
constexpr auto WORLD_SHEET_CENTERPOINT = WORLD_SHEET_SIDELENGTH / 2;

struct DingInstance {
	Ding ding;
	DingProps props;
	
	/// <summary>
	/// grid position of the placed ding's own anchor point on the world sheet
	/// moves with the instance, when it moves relative to the sheet it's on
	/// </summary>
	int gridAnchorX;

	/// <summary>
	/// grid position of the placed ding's own anchor point on the world sheet
	/// moves with the instance, when it moves relative to the sheet it's on
	/// </summary>
	int gridAnchorY;

	/// <summary>
	/// pixel offset from the grid unit where the anchor point is on,
	/// for fine structure position adjustment
	/// </summary>
	int pixOffsetX;

	/// <summary>
	/// pixel offset from the grid unit where the anchor point is on,
	/// for fine structure position adjustment
	/// </summary>
	int pixOffsetY;
};

/// <summary>
/// One grid unit full of information.
/// Contains data for all three layers
/// Multi-cell objects are repeated as
/// pointers in every neighboring cell
/// </summary>
typedef std::vector<DingInstance *> WorldPieces;

/// <summary>
/// The instance storage of all the Worldpieces
/// </summary>
typedef std::vector<DingInstance> WorldPieceRepository;

/// <summary>
/// Use arena to look up the coarse collision environment
/// Use the ding instances inside 
/// </summary>
typedef struct WorldSheet
{
	/// <summary>
	/// How many sheets from the center sheet
	/// </summary>
	int originDx;

	/// <summary>
	/// How many sheets from the center sheet
	/// </summary>
	int originDy;

	/// <summary>
	/// All entities live here. All pointers to said entities are obtained
	/// by emplacing_back here
	/// </summary>
	WorldPieceRepository stuff;

	/// <summary>
	/// Directly adressable x-y-wise by grid unit index in world sheet sidelength stride
	/// </summary>
	std::array<WorldPieces, WORLD_SHEET_SIDELENGTH * WORLD_SHEET_SIDELENGTH> arena;
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
	//WorldRegion* belongsTo = NULL;
} WorldRegion;

void ClearWorldData();
void ClearWorldSheet(int sheetIndex);
void AddRegion(WorldRegion regionDescriptor);
void ReplaceWorldSheet(int sheetX, int sheetY, WorldSheet* content);
WorldPieces GetPiecesRelative(int worldX, int worldY);
const char* GetRegionName(int worldX, int worldY);