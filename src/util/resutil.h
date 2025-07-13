#pragma once
#include "SDL3/SDL.h"

#ifndef NDEBUG
#include <filesystem>
#endif

namespace blooDot::Res
{
	SDL_Texture* LoadPicture(SDL_Renderer* renderer, int chunkKey, SDL_Rect* dimensions);
	SDL_Texture* LoadPicture(SDL_Renderer* renderer, SDL_IOStream* reader, SDL_Rect* dimensions);
	bool LoadText(int chunkKey, std::string& text);

	SDL_Texture* LoadPictureInternal(SDL_Renderer* renderer, SDL_Surface* surface, SDL_Rect* dimensions);
}