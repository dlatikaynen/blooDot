#pragma once
#include "mapregion.h"

namespace blooDot::Map
{
	bool ProcedurallyGenerateScullery(SDL_Renderer* renderer, std::unique_ptr<StaticMapRegionDescriptor>& region);
	bool ProcedurallyGenerateSnurch(SDL_Renderer* renderer, std::unique_ptr<StaticMapRegionDescriptor>& region);

	bool _PrepareDesigntimeRepresentation(SDL_Renderer* renderer, std::unique_ptr<StaticMapRegionDescriptor>& descriptor);
	void _TeardownDesigntimeRepresentation(std::unique_ptr<StaticMapRegionDescriptor>& descriptor);
}