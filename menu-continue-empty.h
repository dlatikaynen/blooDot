#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cairo.h>
#include "splash.h"

namespace blooDot::MenuContinueEmpty
{
	bool MenuLoop(SDL_Renderer* renderer, const char* literalHint);
}