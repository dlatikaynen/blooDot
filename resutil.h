#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "dexassy.h"

namespace blooDot::Res
{
	SDL_Texture* LoadPicture(SDL_Renderer* renderer, const int chunkKey, SDL_Rect* dimensions);
}