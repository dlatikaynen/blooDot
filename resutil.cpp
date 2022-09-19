#include "pch.h"
#include "resutil.h"

namespace blooDot::Res
{
	/// <summary>
	/// Leaves the stream open
	/// </summary>
	SDL_Texture* LoadPicture(SDL_Renderer* renderer, SDL_RWops* reader, SDL_Rect* dimensions)
	{
		const auto& surface = IMG_LoadPNG_RW(reader);
		if (!surface)
		{
			const auto loadError = IMG_GetError();
			ReportError("Failed to load picture from stream", loadError);
			return nullptr;
		}

		return _LoadPicture(renderer, surface, dimensions);
	}

	SDL_Texture* LoadPicture(SDL_Renderer* renderer, const int chunkKey, SDL_Rect* dimensions)
	{
		SDL_RWops* resStream;
		const auto resMem = Retrieve(chunkKey, &resStream);
		if (!resMem)
		{
			return nullptr;
		}

		const auto& resPicture = IMG_LoadPNG_RW(resStream);
		if (!resPicture)
		{
			const auto resLoadError = IMG_GetError();
			resStream->close(resStream);
			SDL_free(resMem);
			ReportError("Failed to load picture from resource", resLoadError);
			return nullptr;
		}

		resStream->close(resStream);
		SDL_free(resMem);

		return _LoadPicture(renderer, resPicture, dimensions);
	}

	SDL_Texture* _LoadPicture(SDL_Renderer* renderer, SDL_Surface* surface, SDL_Rect* dimensions)
	{
		const auto& resTexture = SDL_CreateTextureFromSurface(renderer, surface);
		if (!resTexture)
		{
			const auto textureError = SDL_GetError();
			SDL_free(surface);
			ReportError("Failed to create texture from picture", textureError);
			return nullptr;
		}

		auto& layoutRect = *dimensions;
		layoutRect.x = 0;
		layoutRect.y = 0;
		layoutRect.w = surface->w;
		layoutRect.h = surface->h;
		SDL_free(surface);

		return resTexture;
	}
}