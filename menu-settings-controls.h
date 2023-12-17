#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>
#include "splash.h"
#include "enums.h"
#include "controller-stuff.h"

namespace blooDot::MenuSettingsControls
{
	bool ControlsSettingsMenuLoop(SDL_Renderer*);

	void _PrepareControls(SDL_Renderer* renderer);
	void _DrawController(cairo_t* drawingSink, int xMult, SDL_Color color);
	void _VignetteLabel(SDL_Renderer* renderer, int font, int size, int vignetteIndex, int y, const char* text);
	void _AnimateCarousel();
	void _Teardown();
}