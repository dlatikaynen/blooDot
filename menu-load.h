#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cairo.h>
#include "splash.h"
#include "enums.h"
#include "savegame.h"

extern SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight);
extern bool GpuChanLoop(SDL_Renderer* renderer, const char* message, const char* title, std::string bubble);

namespace blooDot::MenuLoad
{
	bool MenuLoop(SDL_Renderer*);

	void _PrepareControls(SDL_Renderer* renderer);
	void _VignetteLabel(SDL_Renderer* renderer, int font, int size, int vignetteIndex, int y, const char* text);
	void _AnimateCarousel();
	void _Teardown();
}