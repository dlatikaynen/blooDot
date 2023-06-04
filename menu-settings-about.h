#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cairo.h>
#include "splash.h"
#include "enums.h"

extern SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight);

namespace blooDot::MenuSettingsAbout
{
	bool MenuLoop(SDL_Renderer*);

	void _PrepareText(SDL_Renderer* renderer);
	void _Teardown();
}