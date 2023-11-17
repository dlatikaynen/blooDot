#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>
#include "splash.h"

namespace blooDot::MenuCreatorMode
{
	bool MenuLoop(SDL_Renderer*);

	void _PrepareText(SDL_Renderer* renderer, bool destroy = false);
	bool _EnterAndHandleCreatorModeMenu(/*SDL_Renderer* renderer*/);
}