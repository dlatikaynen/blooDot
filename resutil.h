#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "dexassy.h"

namespace blooDot::Res
{
	SDL_Texture* LoadPicture(SDL_Renderer* renderer, const int chunkKey, SDL_Rect* dimensions);
	SDL_Texture* LoadPicture(SDL_Renderer* renderer, SDL_RWops* reader, SDL_Rect* dimensions);
	bool LoadText(const int chunkKey, std::string& text);

	SDL_Texture* _LoadPicture(SDL_Renderer* renderer, SDL_Surface* surface, SDL_Rect* dimensions);
}