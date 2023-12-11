#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>
#include "splash.h"
#include "enums.h"

//extern SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight);

namespace blooDot::MenuSettingsControls
{
	bool ControlsSettingsMenuLoop(SDL_Renderer*);

	void _StatusQuo();
	void _PrepareControls(SDL_Renderer* renderer);
	void _VignetteLabel(SDL_Renderer* renderer, int font, int size, int vignetteIndex, int y, const char* text);
	void _AnimateCarousel();
	void _Teardown();
}