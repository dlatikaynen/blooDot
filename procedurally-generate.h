#pragma once
#include "mapregion.h"

namespace blooDot::Map
{
	bool ProcedurallyGenerateScullery(std::unique_ptr<StaticMapRegionDescriptor>& region);
	bool ProcedurallyGenerateSnurch(std::unique_ptr<StaticMapRegionDescriptor>& region);

	bool _PrepareDesigntimeRepresentation(std::unique_ptr<StaticMapRegionDescriptor>& descriptor);
	void _TeardownDesigntimeRepresentation(std::unique_ptr<StaticMapRegionDescriptor>& descriptor);
}