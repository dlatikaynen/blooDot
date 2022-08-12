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

void SplashLoop(SDL_Renderer* renderer)
{
	LoadSplash(renderer);

	SDL_Rect srcRect{ 0,0,640,480 };
	SDL_Rect dstRect{ 0,0,640,480 };
	SDL_Rect outerMenuRect{ 150,45,340,390 };
	SDL_Rect titleRect{ 0,0,0,0 };
	SDL_Rect continueRect{ 0,0,0,0 };
	SDL_Rect loadRect{ 0,0,0,0 };
	SDL_Rect singleRect{ 0,0,0,0 };
	SDL_Rect localMultiRect{ 0,0,0,0 };
	SDL_Rect netMultiRect{ 0,0,0,0 };
	SDL_Rect settingsRect{ 0,0,0,0 };
	SDL_Rect quitRect{ 0,0,0,0 };

	const auto titleTexture = RenderText(
		renderer,
		&titleRect,
		FONT_KEY_ALIEN,
		16,
		"MainMenu",
		{ 250,200,200,222 }
	);

	/* menu item text */
	const auto continueTexture = RenderText(
		renderer,
		&continueRect,
		FONT_KEY_DIALOG,
		23,
		"Continue",
		{ 250, 230, 230, 245 }
	);

	const auto loadTexture = RenderText(
		renderer,
		&loadRect,
		FONT_KEY_DIALOG,
		23,
		"Load",
		{ 250, 230, 230, 245 }
	);

	const auto singleTexture = RenderText(
		renderer,
		&singleRect,
		FONT_KEY_DIALOG,
		23,
		"Single player",
		{ 250, 230, 230, 245 }
	);

	const auto localMultiTexture = RenderText(
		renderer,
		&localMultiRect,
		FONT_KEY_DIALOG,
		23,
		"Local multiplayer",
		{ 250, 230, 230, 245 }
	);

	const auto netMultiTexture = RenderText(
		renderer,
		&netMultiRect,
		FONT_KEY_DIALOG,
		23,
		"Net multiplayer",
		{ 250, 230, 230, 245 }
	);

	const auto settingsTexture = RenderText(
		renderer,
		&settingsRect,
		FONT_KEY_DIALOG,
		23,
		"Settings",
		{ 250, 230, 230, 245 }
	);

	const auto quitTexture = RenderText(
		renderer,
		&quitRect,
		FONT_KEY_DIALOG,
		23,
		"Exit",
		{ 250, 230, 230, 245 }
	);

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
		
		DrawLabel(renderer, titleTexture, 284, 54, &titleRect);

		const auto drawingTexture = BeginRenderDrawing(renderer);
		if (drawingTexture)
		{
			const auto drawingSink = GetDrawingSink();
			DrawButton(drawingSink, 195, 100, 250, 42);
			DrawButton(drawingSink, 195, 148, 250, 42);
			DrawButton(drawingSink, 195, 196, 250, 42);
			DrawButton(drawingSink, 195, 250, 250, 42);
			DrawButton(drawingSink, 195, 300, 250, 42);
			DrawButton(drawingSink, 195, 350, 250, 42);
			DrawButton(drawingSink, 195, 400, 250, 42);
			EndRenderDrawing(renderer, drawingTexture);

			DrawLabel(renderer, continueTexture, 235, 70 + 36, &continueRect);
			DrawLabel(renderer, loadTexture, 235, 70 + 56 + 50, &loadRect);
			DrawLabel(renderer, singleTexture, 235, 70 + 56 + 100, &singleRect);
			DrawLabel(renderer, localMultiTexture, 235, 70 + 56 + 150, &localMultiRect);
			DrawLabel(renderer, netMultiTexture, 235, 70 + 56 + 200, &netMultiRect);
			DrawLabel(renderer, settingsTexture, 235, 70 + 56 + 250, &settingsRect);
			DrawLabel(renderer, quitTexture, 235, 70 + 56 + 300, &quitRect);
		}

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