#pragma once
#include <SDL2/SDL.h>
//#include <SDL_render.h>

namespace blooDot::Hud
{
	bool Initialize();
	bool Redraw(int ofPlayerIndex);
	void Render();
	void Teardown();

	bool _DrawLetterboxBackdrops(int height);
}