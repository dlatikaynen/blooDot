#include "pch.h"
#include <iostream>
#include "scripture.h"
#include "dexassy.h"

TTF_Font* titleFont = NULL;
SDL_RWops* titleFontSource = NULL;
void* titleFontMem = NULL;

TTF_Font* dialogFont = NULL;
SDL_RWops* dialogFontSource = NULL;
void* dialogFontMem = NULL;

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

	if (!titleFont)
	{
		titleFontMem = LoadFontInternal(CHUNK_KEY_FONT_TITLE, &titleFont, &titleFontSource);
		if (!titleFontMem)
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

	const auto loadedFont = TTF_OpenFontRW(chunkStream, 0, 16);
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
	case FONT_KEY_TITLE:
		return titleFont;

	case FONT_KEY_DIALOG:
		return dialogFont;

	case FONT_KEY_ALIEN:
		return alienFont;
	}

	return NULL;
}

SDL_Texture* RenderText(SDL_Renderer* renderer, SDL_Rect* frame, int fontKey, int sizePt, const char* text, SDL_Color color)
{
	SDL_Texture* textTexture = NULL;

	const auto font = GetFont(fontKey);
	TTF_SetFontSize(dialogFont, sizePt);
	const auto textSurface = TTF_RenderUTF8_Blended(font, text, color);

	if (textSurface)
	{
		(*frame).w = textSurface->w;
		(*frame).h = textSurface->h;
		textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (!textTexture)
		{
			const auto textureError = SDL_GetError();
			ReportError("Failed to create a text texture", textureError);

			return NULL;
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
	if (titleFont)
	{
		TTF_CloseFont(titleFont);
		titleFontSource->close(titleFontSource);
		SDL_free(titleFontMem);
		titleFont = NULL;
	}

	if (dialogFont)
	{
		TTF_CloseFont(dialogFont);
		dialogFontSource->close(dialogFontSource);
		SDL_free(dialogFontMem);
		dialogFont = NULL;
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
