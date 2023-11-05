#pragma once
#include <SDL2/SDL.h>
#include "gamestate.h"
#include "mapregion.h"

extern void ReportError(const char*, const char*);

using namespace blooDot::Map;

namespace blooDot::World
{
	bool InitializeNewWorld(SDL_Renderer* renderer);

	void _InstantiateStaticRegionOnWorldsheet(std::shared_ptr<WorldSheet>& worldSheet, std::unique_ptr<StaticMapRegionDescriptor>& region);
	void _Put(std::shared_ptr<WorldSheet> sheet, int x, int y, Ding ding, short pixX = 0, short pixY = 0, short angle = 0, DingProps props = DingProps::Default);
}