#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>
#include "splash.h"

namespace blooDot::MenuContinueEmpty
{
	bool MenuLoop(SDL_Renderer* renderer, const char* literalHint);
}