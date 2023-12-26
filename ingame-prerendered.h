#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>

namespace blooDot::InGamePreRendered
{
	extern SDL_Texture* PreRendered;

	constexpr const double InGameControlMargin = 10;
	constexpr const double InGameSkipButtonWidth = 161;
	constexpr const double InGameButtonHeight = 42;

	bool PreRender(SDL_Renderer*);
	void Teardown();
}