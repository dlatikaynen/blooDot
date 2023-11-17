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

void* LoadFontInternal(int chunkKey, __out TTF_Font** font, __out SDL_RWops **source)
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
	}
	else
	{
		const auto textError = TTF_GetError();
		ReportError("Failed to render text", textError);
		return NULL;
	}

	SDL_free(textSurface);

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
