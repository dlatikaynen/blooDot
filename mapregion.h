#pragma once
#include <vector>
#include "gamestate.h"
#include <memory>

namespace blooDot::Map
{
	/* describes a single map region as it appears on the map.
	 * does not include the absolute position inside the world.
	 * represents that region in the state in which it is shipped
	 * with the game, and as it is initialized when a new game
	 * is started.
	 * represents the storage format of that data in what we
	 * might call "level files" */

	typedef struct StaticDingPlacementStruct
	{
		Ding ding = Ding::None;
		DingProps props = DingProps::Default;

		/// <summary>
		/// grid position in the region (0 is center)
		/// </summary>
		int fromOriginX = 0;
		int fromOriginY = 0;

		/// <summary>
		/// fine tuning of the placement
		/// </summary>
		int pixOffsetX = 0;
		int pixOffsetY = 0;
		int rotationAngle = 0;
	} DingPlacement;

	typedef struct StaticMapRegionDescriptorStruct {
		WorldRegion region;
		std::vector<DingPlacement> dingPlacement;
	} StaticMapRegionDescriptor;

	bool _LoadStaticRegion(int regionId, std::unique_ptr<StaticMapRegionDescriptor>& descriptor);
	void _Define(std::unique_ptr<StaticMapRegionDescriptor> region, int x, int y, int pixX, int pixY, int angle, Ding ding, DingProps props);
	bool _WriteStaticRegion(int regionId, std::unique_ptr<StaticMapRegionDescriptor>& descriptor);
}