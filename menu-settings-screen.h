#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>
#include "splash.h"
#include "boydsa.h"
#include "enums.h"
#include "sfx.h"
#include "resutil.h"
#include "geom2d.h"

extern SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight);
extern bool GpuChanLoop(SDL_Renderer* renderer, const char* message, const char* title, std::string bubble);

namespace blooDot::MenuSettingsScreen
{
	bool ScreenSettingsMenuLoop(SDL_Renderer*);

	void _PrepareControls(SDL_Renderer* renderer);
	void _VignetteLabel(SDL_Renderer* renderer, int font, int size, int vignetteIndex, int y, const char* text);
	void _AnimateCarousel();
	void _Teardown();
	bool _CanSelectMode(SDL_Renderer* renderer);
	bool _GetResolution(ViewportResolutions videoMode, SDL_Rect* dimensions);
}