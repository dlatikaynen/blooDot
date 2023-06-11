#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>
#include "splash.h"
#include "enums.h"

extern SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight);

namespace blooDot::MenuSettingsHelp
{
	bool HelpMenuLoop(SDL_Renderer*);

	void _PrepareControls(SDL_Renderer* renderer);
	void _Teardown();
}