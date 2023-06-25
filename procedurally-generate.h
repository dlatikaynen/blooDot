#pragma once
#include "mapregion.h"

namespace blooDot::Map
{
	void ProcedurallyGenerateScullery(std::unique_ptr<StaticMapRegionDescriptor>& region);
	void ProcedurallyGenerateSnurch(std::unique_ptr<StaticMapRegionDescriptor>& region);
}