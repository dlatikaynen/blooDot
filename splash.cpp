#include "pch.h"
#include "splash.h"
#include "dexassy.h"
#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo.h>

extern bool mainRunning;

SDL_Event splashEvent;
bool splashRunning = false;
SDL_Texture* splashTexture = NULL;

void LoadSplash(SDL_Renderer* renderer)
{
	if (!LoadFonts())
	{
		mainRunning = false;
		return;
	}

	SDL_RWops* splashStream;
	const auto splashMem = Retrieve(CHUNK_KEY_SPLASH, &splashStream);
	if (!splashMem)
	{
		mainRunning = false;
		return;
	}

	const auto splashPicture = IMG_LoadPNG_RW(splashStream);
	splashStream->close(splashStream);
	SDL_free(splashMem);
	if (!splashPicture)
	{
		const auto splashLoadError = IMG_GetError();
		ReportError("Failed to load the title screen", splashLoadError);
		mainRunning = false;
		return;
	}

	splashTexture = SDL_CreateTextureFromSurface(renderer, splashPicture);
	SDL_free(splashPicture);
	if (!splashTexture)
	{
		const auto textureError = SDL_GetError();
		ReportError("Failed to create the title screen texture", textureError);
		mainRunning = false;
		return;
	}

	splashRunning = true;
}

cairo_t* cb(cairo_t* cr)
{
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_set_line_width(cr, 10.0);
	cairo_rectangle(cr, 10, 20, 128, 128);
	cairo_stroke(cr);
	return cr;
}

void SplashLoop(SDL_Renderer* renderer)
{
	LoadSplash(renderer);

	SDL_Rect srcRect{ 0,0,640,480 };
	SDL_Rect dstRect{ 0,0,640,480 };
	//SDL_Rect outerMenuRect{ 150,100,640 - 300,480 - 200 };
	SDL_Rect outerMenuRect{ 150,100,300,300 };
	SDL_Rect titleRect{};

	const auto font = GetFont(FONT_KEY_ALIEN);
	SDL_Color fontColor{ 250,200,200,222 };
	const auto titleText = TTF_RenderUTF8_Blended(font, "SiPiaSibasIan", fontColor);
	if (!titleText)
	{
		const auto textError = TTF_GetError();
		ReportError("Failed to render text", textError);
		mainRunning = false;
		splashRunning = false;
	}

	const auto textTexture = SDL_CreateTextureFromSurface(renderer, titleText);
	if (!textTexture)
	{
		const auto textureError = SDL_GetError();
		ReportError("Failed to create the title text texture", textureError);
		mainRunning = false;
		return;
	}

	if (titleText)
	{
		titleRect.x = outerMenuRect.x + 10;
		titleRect.y = outerMenuRect.y + 10;
		titleRect.w = titleText->w;
		titleRect.h = titleText->h;
		SDL_free(titleText);
	}

	const auto primaryController = SDL_GameControllerOpen(0);
	if (primaryController)
	{
		SDL_GameControllerSetPlayerIndex(primaryController, 1);
		SDL_GameControllerRumble(primaryController, 0xffff, 0xffff, 0xff);
		SDL_GameControllerEventState(SDL_ENABLE);
	}

	while (splashRunning)
	{
		while (SDL_PollEvent(&splashEvent) != 0)
		{
			switch (splashEvent.type)
			{
			case SDL_QUIT:
				mainRunning = false;
				splashRunning = false;
				break;

			case SDL_KEYDOWN:
				splashRunning = false;
				break;

			case SDL_CONTROLLERAXISMOTION:
				if (splashEvent.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) 
				{
					srcRect.x += splashEvent.caxis.value / 8192;
				}

				if (splashEvent.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
				{
					srcRect.y += splashEvent.caxis.value / 8192;
				}

				break;
			}
		}

		if (SDL_RenderClear(renderer) < 0)
		{
			const auto clearError = IMG_GetError();
			ReportError("Failed to clear the title screen", clearError);
		}

		if (SDL_RenderCopy(renderer, splashTexture, &srcRect, &dstRect) < 0)
		{
			const auto blitError = SDL_GetError();
			ReportError("Failed to blit splash background", blitError);
			splashRunning = false;
		}

		SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
		SDL_RenderFillRect(renderer, &outerMenuRect);
		SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
		if (SDL_RenderDrawRect(renderer, &outerMenuRect) < 0)
		{
			const auto drawRectError = SDL_GetError();
			ReportError("Failed to draw panel border", drawRectError);
			splashRunning = false;
		};
		
		SDL_RenderCopy(renderer, textTexture, NULL, &titleRect);
		RenderDrawing(renderer, cb);
		RenderDrawing(renderer, DrawButtonChevron);

		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}

	if (primaryController)
	{
		SDL_GameControllerClose(primaryController);
	}

	if (splashTexture)
	{
		SDL_DestroyTexture(splashTexture);
	}
}