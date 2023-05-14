#pragma once
#include <SDL.h>
#include <SDL_render.h>

namespace blooDot::RegionBanner
{
	bool Initialize();
	bool Redraw(int ofRegion);
	void Render();
	void Teardown();
}