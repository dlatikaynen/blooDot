#include "pch.h"
#include "newworld.h"
#include "xlations.h"
#include "procedurally-generate.h"
#include <iostream>

using namespace blooDot::Map;

bool InitializeNewWorld()
{
	/* provisional procedural generation of static
	 * world regions for prototyping purposes */
	auto sculleryTemplate = std::make_unique<StaticMapRegionDescriptor>();
	blooDot::Map::ProcedurallyGenerateScullery(sculleryTemplate);
	blooDot::Map::WriteStaticRegion(sculleryTemplate);
	
	auto sculleryStatic = std::make_unique<StaticMapRegionDescriptor>();
	blooDot::Map::LoadStaticRegion(1, sculleryStatic);

	/* instantiate the regions in our surrounding */
	ClearWorldData();

	WorldRegion gardenOFRidiculouslyPoisonousPlants;
	gardenOFRidiculouslyPoisonousPlants.RegionId = 2;
	gardenOFRidiculouslyPoisonousPlants.RegionName = std::string(literallandmarkNameGardenOfRidiculouslyPoisonousPlants);
	AddRegion(&gardenOFRidiculouslyPoisonousPlants);

	WorldRegion scullery;
	//scullery.belongsTo = NULL;
	scullery.RegionId = sculleryStatic->region.RegionId;
	scullery.RegionName = std::string(literalregionNameScullery);
	for (auto& poly : sculleryStatic->region.polygon)
	{
		scullery.polygon.push_back(poly);
	}

	AddRegion(&scullery);
	
	/* populate from the current region */
	auto centerSheet = GetWorldSheet(0, 0);
	if (!centerSheet)
	{
		const auto allocError = SDL_GetError();
		ReportError("Encountered unallocated center sheet", allocError);
		return false;
	}

	for (auto& placement : sculleryStatic->dingPlacement)
	{
		_Put(
			centerSheet,
			placement.fromOriginX,
			placement.fromOriginY,
			placement.ding,
			placement.props
		);
	}

	return true;
}

void _Put(std::shared_ptr<WorldSheet> sheet, int x, int y, Ding ding, DingProps props)
{
	const auto pieceIndex = (y + WORLD_SHEET_CENTERPOINT) * WORLD_SHEET_SIDELENGTH + x + WORLD_SHEET_CENTERPOINT;
	const auto instancePtr = std::make_shared<DingInstance>();

	/* 1. assign the properties */
	instancePtr->ding = ding;
	instancePtr->props = props == DingProps::Default ? GetDingDefaultProps(ding) : props;
	instancePtr->gridAnchorX = x;
	instancePtr->gridAnchorY = y;

	/* 2. register the entity in the world sheet storage.
	*     this step may become obsolete since they now live on the heap */
	sheet->stuff->push_back(instancePtr);

	/* 3. place its anchor point into the grid cell where it is anchored */
	sheet->arena[pieceIndex].push_back(instancePtr);

	/* 4. TODO: compute all other grid cells which it touches, so collision
	 *    detection via bounding squares will work */

}