#include <iostream>
#include "scripture.h"
#include "../../main.h"
#include "../../xassy/dexassy.h"
#include "../../res/chunk-constants.h"

namespace blooDot::Scripture {
	TTF_Font* fancyFont = nullptr;
	SDL_IOStream* fancyFontSource = nullptr;
	void* fancyFontMem = nullptr;

	TTF_Font* dialogFont = nullptr;
	SDL_IOStream* dialogFontSource = nullptr;
	void* dialogFontMem = nullptr;

	TTF_Font* dialogFontFat = nullptr;
	SDL_IOStream* dialogFontFatSource = nullptr;
	void* dialogFontFatMem = nullptr;

	TTF_Font* alienFont = nullptr;
	SDL_IOStream* alienFontSource = nullptr;
	void* alienFontMem = nullptr;

	bool LoadFonts()
	{
		if (!TTF_WasInit())
		{
			if (!TTF_Init())
			{
				const auto ttfError = SDL_GetError();

				ReportError("Failed to initialize fonts", ttfError);

				return false;
			}

			std::cout
				<< "Font Library Version "
				<< SDL_TTF_MAJOR_VERSION
				<< "."
				<< SDL_TTF_MINOR_VERSION
				<< "."
				<< SDL_TTF_MICRO_VERSION
				<< "\n";

			int barfMaj, barfMin, barfRev;
			TTF_GetHarfBuzzVersion(&barfMaj, &barfMin, &barfRev);

			std::cout
				<< "Harfbuzz Version "
				<< barfMaj
				<< "."
				<< barfMin
				<< "."
				<< barfRev
				<< std::endl;
		}

		if (!fancyFont)
		{
			fancyFontMem = LoadFontInternal(CHUNK_KEY_FONT_FANCY, &fancyFont, &fancyFontSource);
			if (!fancyFontMem)
			{
				return false;
			}
		}

		if (!dialogFont)
		{
			dialogFontMem = LoadFontInternal(CHUNK_KEY_FONT_DIALOG, &dialogFont, &dialogFontSource);
			if (!dialogFontMem)
			{
				return false;
			}
		}

		if (!dialogFontFat)
		{
			dialogFontFatMem = LoadFontInternal(CHUNK_KEY_FONT_DIALOG_FAT, &dialogFontFat, &dialogFontFatSource);
			if (!dialogFontFatMem)
			{
				return false;
			}
		}

		if (!alienFont)
		{
			alienFontMem = LoadFontInternal(CHUNK_KEY_FONT_ALIEN, &alienFont, &alienFontSource);
			if (!alienFontMem)
			{
				return false;
			}
		}

		return true;
	}

	void* LoadFontInternal(const int chunkKey, TTF_Font** font, SDL_IOStream** source)
	{
		SDL_IOStream* chunkStream;
		*font = nullptr;
		*source = nullptr;
		const auto chunkMem = Retrieve(chunkKey, &chunkStream);

		if (!chunkMem)
		{
			return nullptr;
		}

		const auto loadedFont = TTF_OpenFontIO(chunkStream, false, 19);

		if (!loadedFont)
		{
			const auto fontError = SDL_GetError();

			SDL_CloseIO(chunkStream);
			SDL_free(chunkMem);
			ReportError("Failed to load font", fontError);

			return nullptr;
		}

		*font = loadedFont;
		*source = chunkStream;

		return chunkMem;
	}

	TTF_Font* GetFont(int fontKey)
	{
		switch (fontKey)
		{
			case FONT_KEY_DIALOG:
				return dialogFont;

			case FONT_KEY_DIALOG_FAT:
				return dialogFontFat;

			case FONT_KEY_FANCY:
				return fancyFont;

			case FONT_KEY_ALIEN:
				return alienFont;

			default:
				break;
		}

		return nullptr;
	}

	/// <summary>
	/// Draws text on an existing texture using an intermediary surface
	/// Texture must be writable (streaming-enabled)
	/// Text is centered inside the rectange "frame" on the texture,
	/// where the co-ordinates given by "frame" must be fully inside texture.
	/// </summary>
	bool DrawText(SDL_Texture* streamingDestination, const SDL_Rect* frame, const int fontKey, const int sizePt, const char* text, SDL_Color color, const bool bold)
	{
		SDL_Surface* writeThrough;
		if (SDL_LockTextureToSurface(streamingDestination, frame, &writeThrough) != 0)
		{
			const auto retrieveError = SDL_GetError();

			ReportError("Could not lock texture to surface to write on", retrieveError);

			return false;
		}

		const auto font = GetFont(fontKey);
		if (!TTF_SetFontSize(font, static_cast<float>(sizePt)))
		{
			const auto sizeError = SDL_GetError();

			ReportError("Failed to set font size", sizeError);
			SDL_UnlockTexture(streamingDestination);

			return false;
		}

		TTF_SetFontStyle(font, bold ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL);
		const auto textSurface = TTF_RenderText_Blended_Wrapped(
			font,
			text,
			strnlen(text, 0xffffffff),
			color,
			0
		);

		if (textSurface == nullptr)
		{
			const auto textError = SDL_GetError();

			ReportError("Failed to render text", textError);
			SDL_UnlockTexture(streamingDestination);

			return false;
		}

		const SDL_Rect src = { 0,0,textSurface->w,textSurface->h };

		const SDL_Rect dst = {
			frame->w / 2 - textSurface->w / 2,
			frame->h / 2 - textSurface->h / 2 - 2, // -2 empirically determined
			textSurface->w,
			textSurface->h
		};

		if (SDL_BlitSurface(textSurface, &src, writeThrough, &dst) != 0)
		{
			const auto blitError = SDL_GetError();

			ReportError("Failed to blit from text to write-through surface", blitError);
			SDL_free(textSurface);
			SDL_UnlockTexture(streamingDestination);

			return false;
		}

		SDL_free(textSurface);
		SDL_UnlockTexture(streamingDestination);

		return true;
	}

	/// <summary>
	/// Creates a new texture with the text written on it.
	/// Can only be used to SDL_RenderCopy directly to a render target.
	/// Use the DrawText method instead, if you need to paint text on
	/// top of anything prior existing as (and on) a texture.
	/// </summary>
	SDL_Texture* RenderText(SDL_Renderer* renderer, SDL_FRect* frame, int fontKey, int sizePt, const char* text, SDL_Color color, bool bold)
	{
		SDL_Texture* textTexture = nullptr;

		const auto font = GetFont(fontKey);
		if (!TTF_SetFontSize(font, static_cast<float>(sizePt)))
		{
			const auto sizeError = SDL_GetError();
			ReportError("Failed to set font size", sizeError);
		}

		TTF_SetFontStyle(font, bold ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL);
		const auto textSurface = TTF_RenderText_Blended_Wrapped(
			font,
			text,
			strnlen(text, 0xffffffff),
			color,
			0
		);

		if (textSurface)
		{
			frame->w = static_cast<float>(textSurface->w);
			frame->h = static_cast<float>(textSurface->h);
			textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
			if (textTexture == nullptr)
			{
				const auto textureError = SDL_GetError();

				ReportError("Failed to create a text texture", textureError);
			}

			SDL_free(textSurface);
		}
		else
		{
			const auto textError = SDL_GetError();

			ReportError("Failed to render text", textError);

			return nullptr;
		}

		return textTexture;
	}

	void CloseFonts()
	{
		if (fancyFont)
		{
			TTF_CloseFont(fancyFont);
			SDL_CloseIO(fancyFontSource);
			SDL_free(fancyFontMem);
			fancyFont = nullptr;
		}

		if (dialogFont)
		{
			TTF_CloseFont(dialogFont);
			SDL_CloseIO(dialogFontSource);
			SDL_free(dialogFontMem);
			dialogFont = nullptr;
		}

		if (dialogFontFat)
		{
			TTF_CloseFont(dialogFontFat);
			SDL_CloseIO(dialogFontFatSource);
			SDL_free(dialogFontFatMem);
			dialogFontFat = nullptr;
		}

		if (alienFont)
		{
			TTF_CloseFont(alienFont);
			SDL_CloseIO(alienFontSource);
			SDL_free(alienFontMem);
			alienFont = nullptr;
		}

		if (TTF_WasInit())
		{
			TTF_Quit();
		}
	}
}