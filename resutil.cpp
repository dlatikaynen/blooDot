#include "pch.h"
#include "resutil.h"

namespace blooDot::Res
{
	SDL_Texture* LoadPicture(SDL_Renderer* renderer, const int chunkKey, SDL_Rect* dimensions)
	{
		SDL_RWops* resStream;
		const auto resMem = Retrieve(chunkKey, &resStream);
		if (!resMem)
		{
			return nullptr;
		}

		const auto& resPicture = IMG_LoadPNG_RW(resStream);
		resStream->close(resStream);
		SDL_free(resMem);
		if (!resPicture)
		{
			const auto resLoadError = IMG_GetError();
			ReportError("Failed to load picture from resource", resLoadError);
			return nullptr;
		}

		const auto& resTexture = SDL_CreateTextureFromSurface(renderer, resPicture);
		if (!resTexture)
		{
			const auto textureError = SDL_GetError();
			SDL_free(resPicture);
			ReportError("Failed to create texture from resource", textureError);
			return nullptr;
		}

		auto& layoutRect = *dimensions;
		layoutRect.x = 0;
		layoutRect.y = 0;
		layoutRect.w = resPicture->w;
		layoutRect.h = resPicture->h;
		SDL_free(resPicture);

		return resTexture;
	}
}