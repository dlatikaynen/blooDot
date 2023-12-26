#include "pch.h"
#include <iostream>
#include "scripture.h"
#include "dexassy.h"

TTF_Font* fancyFont = NULL;
SDL_RWops* fancyFontSource = NULL;
void* fancyFontMem = NULL;

TTF_Font* dialogFont = NULL;
SDL_RWops* dialogFontSource = NULL;
void* dialogFontMem = NULL;

TTF_Font* dialogFontFat = NULL;
SDL_RWops* dialogFontFatSource = NULL;
void* dialogFontFatMem = NULL;

TTF_Font* alienFont = NULL;
SDL_RWops* alienFontSource = NULL;
void* alienFontMem = NULL;

bool LoadFonts()
{
	if (!TTF_WasInit()) 
	{
		if (TTF_Init() < 0)
		{
			const auto ttfError = TTF_GetError();
			ReportError("Failed to initialize fonts", ttfError);
			return false;
		}

		const auto ttfVersion = TTF_Linked_Version();
		std::cout 
			<< "Font Library Version "
			<< unsigned(ttfVersion->major)
			<< "."
			<< unsigned(ttfVersion->minor)
			<< "."
			<< unsigned(ttfVersion->patch)
			<< "\n";
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

void* LoadFontInternal(int chunkKey, __out TTF_Font** font, __out SDL_RWops** source)
{
	SDL_RWops* chunkStream;
	*font = NULL;
	*source = NULL;
	const auto chunkMem = Retrieve(chunkKey, &chunkStream);
	if (!chunkMem)
	{
		return NULL;
	}

	const auto loadedFont = TTF_OpenFontRW(chunkStream, 0, 19);
	if (!loadedFont)
	{
		const auto fontError = TTF_GetError();
		SDL_free(chunkMem);
		ReportError("Failed to load font", fontError);
		return NULL;
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
	}

	return NULL;
}

/// <summary>
/// Draws text on an existing texture using an intermediary surface
/// Texture must be writable (streaming-enabled)
/// Text is centered inside the rectange "frame" on the texture,
/// where the co-ordinates given by "frame" must be fully inside texture.
/// </summary>
bool DrawText(SDL_Texture* streamingDestination, SDL_Rect* frame, int fontKey, int sizePt, const char* text, SDL_Color color, bool bold)
{
	SDL_Surface* writeThrough;
	if (SDL_LockTextureToSurface(streamingDestination, frame, &writeThrough) != 0)
	{
		const auto retrieveError = SDL_GetError();
		ReportError("Could not lock texture to surface to write on", retrieveError);

		return false;
	}

	const auto font = GetFont(fontKey);
	if (TTF_SetFontSize(font, sizePt) < 0)
	{
		const auto sizeError = TTF_GetError();
		ReportError("Failed to set font size", sizeError);
		SDL_UnlockTexture(streamingDestination);

		return false;
	}

	TTF_SetFontStyle(font, bold ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL);
	const auto textSurface = TTF_RenderUTF8_Blended_Wrapped(font, text, color, 0);
	if (textSurface == nullptr)
	{
		const auto textError = TTF_GetError();
		ReportError("Failed to render text", textError);
		SDL_UnlockTexture(streamingDestination);

		return false;
	}

	const SDL_Rect src = { 0,0,textSurface->w,textSurface->h };
	SDL_Rect dst = {
		frame->w / 2 - textSurface->w / 2,
		frame->h / 2 - textSurface->h / 2 - 2, // -2 empirically determined
		textSurface->w,
		textSurface->h 
	};

	if (SDL_BlitSurface(textSurface, &src, writeThrough, &dst) != 0)
	{
		const auto blitError = TTF_GetError();
		ReportError("Failed to blit from text to write-through wurface", blitError);
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
SDL_Texture* RenderText(SDL_Renderer* renderer, SDL_Rect* frame, int fontKey, int sizePt, const char* text, SDL_Color color, bool bold)
{
	SDL_Texture* textTexture = NULL;

	const auto font = GetFont(fontKey);
	if(TTF_SetFontSize(font, sizePt) < 0)
	{ 
		const auto sizeError = TTF_GetError();
		ReportError("Failed to set font size", sizeError);
	}

	TTF_SetFontStyle(font, bold ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL);
	const auto textSurface = TTF_RenderUTF8_Blended_Wrapped(font, text, color, 0);

	if (textSurface)
	{
		(*frame).w = textSurface->w;
		(*frame).h = textSurface->h;
		textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (textTexture == NULL)
		{
			const auto textureError = SDL_GetError();
			ReportError("Failed to create a text texture", textureError);
		}

		SDL_free(textSurface);
	}
	else
	{
		const auto textError = TTF_GetError();
		ReportError("Failed to render text", textError);
		return NULL;
	}

	return textTexture;
}

void CloseFonts()
{
	if (fancyFont)
	{
		TTF_CloseFont(fancyFont);
		fancyFontSource->close(fancyFontSource);
		SDL_free(fancyFontMem);
		fancyFont = NULL;
	}

	if (dialogFont)
	{
		TTF_CloseFont(dialogFont);
		dialogFontSource->close(dialogFontSource);
		SDL_free(dialogFontMem);
		dialogFont = NULL;
	}

	if (dialogFontFat)
	{
		TTF_CloseFont(dialogFontFat);
		dialogFontFatSource->close(dialogFontFatSource);
		SDL_free(dialogFontFatMem);
		dialogFontFat = NULL;
	}

	if (alienFont)
	{
		TTF_CloseFont(alienFont);
		alienFontSource->close(alienFontSource);
		SDL_free(alienFontMem);
		alienFont = NULL;
	}

	if (TTF_WasInit())
	{
		TTF_Quit();
	}
}
