#include "mapregion.h"

namespace blooDot::Map
{
	bool _LoadStaticRegion(int regionId, std::unique_ptr<StaticMapRegionDescriptor>& descriptor)
	{
		return true;
	}

	void _Define(std::unique_ptr<StaticMapRegionDescriptor> region, int x, int y, int pixX, int pixY, int angle, Ding ding, DingProps props)
	{
		auto& placement = DingPlacement();
		placement.ding = ding;
		placement.fromOriginX = x;
		placement.fromOriginY = y;
		
		region->dingPlacement.push_back(placement);
	}

	bool _WriteStaticRegion(int regionId, std::unique_ptr<StaticMapRegionDescriptor>& descriptor)
	{
		return true;
	}
}