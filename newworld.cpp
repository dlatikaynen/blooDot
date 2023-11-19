#include "pch.h"
#include "newworld.h"
#include "xlations.h"
#include "procedurally-generate.h"
#include <iostream>

using namespace blooDot::Map;

namespace blooDot::World
{
	bool InitializeNewWorld(SDL_Renderer* renderer)
	{
		/* provisional procedural generation of static
		 * world regions for prototyping purposes */

		/* instantiate the regions in our surrounding */
		ClearWorldData();

		/* garden instantiation */
		WorldRegion gardenOFRidiculouslyPoisonousPlants;
		gardenOFRidiculouslyPoisonousPlants.RegionId = 2;
		gardenOFRidiculouslyPoisonousPlants.RegionName = std::string(literallandmarkNameGardenOfRidiculouslyPoisonousPlants);
		AddRegion(&gardenOFRidiculouslyPoisonousPlants);

		/* scullery instantiation */
		auto sculleryTemplate = std::make_unique<StaticMapRegionDescriptor>();
		blooDot::Map::ProcedurallyGenerateScullery(renderer, sculleryTemplate);
		blooDot::Map::WriteStaticRegion(sculleryTemplate);

		auto sculleryStatic = std::make_unique<StaticMapRegionDescriptor>();
		blooDot::Map::LoadStaticRegion(sculleryTemplate->region.RegionId, sculleryStatic);

		WorldRegion scullery;
		scullery.RegionId = sculleryStatic->region.RegionId;
		scullery.RegionName = std::string(literalregionNameScullery);
		for (auto& poly : sculleryStatic->region.polygon)
		{
			scullery.polygon.push_back(poly);
		}

		AddRegion(&scullery);

		/* snurch instantiation */
		auto snurchTemplate = std::make_unique<StaticMapRegionDescriptor>();
		blooDot::Map::ProcedurallyGenerateSnurch(renderer, snurchTemplate);
		blooDot::Map::WriteStaticRegion(snurchTemplate);

		auto snurchStatic = std::make_unique<StaticMapRegionDescriptor>();
		blooDot::Map::LoadStaticRegion(snurchTemplate->region.RegionId, snurchStatic);

		WorldRegion snurch;
		snurch.RegionId = snurchStatic->region.RegionId;
		snurch.RegionName = std::string(literallandmarkNameSnurch);
		for (auto& poly : snurchStatic->region.polygon)
		{
			snurch.polygon.push_back(poly);
		}

		AddRegion(&snurch);

		/* populate from the current region */
		auto centerSheet = GetWorldSheet(0, 0);
		if (!centerSheet)
		{
			const auto allocError = SDL_GetError();
			ReportError("Encountered unallocated center sheet", allocError);
			return false;
		}

		_InstantiateStaticRegionOnWorldsheet(centerSheet, sculleryStatic);
		//_InstantiateStaticRegionOnWorldsheet(centerSheet, snurchStatic);

		return true;
	}

	void _InstantiateStaticRegionOnWorldsheet(std::shared_ptr<WorldSheet>& worldSheet, std::unique_ptr<blooDot::Map::StaticMapRegionDescriptor>& region)
	{
		for (auto& placement : region->dingPlacement)
		{
			_Put(
				worldSheet,
				placement.fromOriginX,
				placement.fromOriginY,
				placement.ding,
				placement.pixOffsetX,
				placement.pixOffsetY,
				placement.rotationAngle,
				placement.zIndex,
				placement.props
			);
		}
	}

	void _Put(std::shared_ptr<WorldSheet> sheet, int x, int y, Ding ding, short pixX, short pixY, short angle, short zIndex, DingProps props)
	{
		const auto pieceIndex = (y + WORLD_SHEET_CENTERPOINT) * WORLD_SHEET_SIDELENGTH + x + WORLD_SHEET_CENTERPOINT;
		const auto instancePtr = std::make_shared<DingInstance>();

		/* 1. assign the properties */
		instancePtr->ding = ding;
		instancePtr->props = props == DingProps::Default ? GetDingDefaultProps(ding) : props;
		instancePtr->gridAnchorX = x;
		instancePtr->gridAnchorY = y;
		instancePtr->pixOffsetX = pixX;
		instancePtr->pixOffsetY = pixY;
		instancePtr->rotationAngle = angle;
		instancePtr->zIndex = zIndex;

		/* 2. register the entity in the world sheet storage.
		*     this step may become obsolete since they now live on the heap */
		sheet->stuff->push_back(instancePtr);

		/* 3. place its anchor point into the grid cell where it is anchored */
		sheet->arena[pieceIndex].push_back(instancePtr);

		/* 4. TODO: compute all other grid cells which it touches, so collision
		 *    detection via bounding squares will work */

	}
}