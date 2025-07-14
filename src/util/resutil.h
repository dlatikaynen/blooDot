#pragma once
#include "SDL3/SDL.h"
#include <filesystem>

namespace blooDot::Res
{
	SDL_Texture* LoadPicture(SDL_Renderer* renderer, int chunkKey, SDL_FRect* dimensions);
	SDL_Texture* LoadPicture(SDL_Renderer* renderer, SDL_IOStream* reader, SDL_FRect* dimensions);
	bool LoadText(int chunkKey, std::string& text);

	SDL_Texture* LoadPictureInternal(SDL_Renderer* renderer, SDL_Surface* surface, SDL_FRect* dimensions);
}