#pragma once
#include <SDL.h>
#include <box2d.h>
#include "ding.h"
#include "enums.h"

constexpr auto WORLD_SHEET_SIDELENGTH = 200;
constexpr auto WORLD_SHEET_CENTERPOINT = WORLD_SHEET_SIDELENGTH / 2;

struct DingInstance
{
	Ding ding = Ding::None;
	DingProps props = DingProps::Default;
	
	/// <summary>
	/// grid position of the placed ding's own anchor point on the world sheet
	/// moves with the instance, when it moves relative to the sheet it's on
	/// </summary>
	int gridAnchorX = 0;

	/// <summary>
	/// grid position of the placed ding's own anchor point on the world sheet
	/// moves with the instance, when it moves relative to the sheet it's on
	/// </summary>
	int gridAnchorY = 0;

	/// <summary>
	/// The array of convex polygons that define the collision boundary of the
	/// entity in world sheet absolute coordinates. The special case is wall-likes,
	/// where this equals nullptr, then the collision polygon is identical to the
	/// entitie's grid square.
	/// Note that a group of entities can share a common hull collision polygon.
	/// In this case, more than one instance points to the same vector.
	/// </summary>
	std::shared_ptr<std::vector<b2PolygonShape>> collisionPolygon = nullptr;

	/// <summary>
	/// pixel offset from the grid unit where the anchor point is on,
	/// for fine structure position adjustment
	/// </summary>
	int pixOffsetX = 0;

	/// <summary>
	/// pixel offset from the grid unit where the anchor point is on,
	/// for fine structure position adjustment
	/// </summary>
	int pixOffsetY = 0;

	/// <summary>
	/// Angle in integer radians,
	/// for non-rectangular positioning on a worldsheet
	/// </summary>
	int rotationAngle = 0;
};

/// <summary>
/// One grid unit full of information.
/// Contains data for all three layers
/// Multi-cell objects are repeated as
/// pointers in every neighboring cell
/// </summary>
typedef std::vector<std::shared_ptr<DingInstance>> WorldPieces;

/// <summary>
/// The instance storage of all the Worldpieces
/// </summary>
typedef std::vector<std::shared_ptr<DingInstance>> WorldPieceRepoEntry;
typedef std::shared_ptr<WorldPieceRepoEntry> WorldPieceRepository;

/// <summary>
/// Use arena to look up the coarse collision environment
/// Use the ding instances inside 
/// </summary>
typedef class WorldSheet
{
public:
	WorldSheet()
	{
		auto newStuff = std::make_shared<WorldPieceRepoEntry>();
		this->stuff = newStuff;
	}

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
std::shared_ptr<WorldSheet> GetWorldSheet(int sheetX, int sheetY);
WorldPieces* GetPiecesRelative(int worldX, int worldY);
const char* GetRegionName(int worldX, int worldY);