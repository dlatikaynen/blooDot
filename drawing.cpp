#include "pch.h"
#include "drawing.h"
#include "constants.h"

cairo_t* drawingSink = NULL;

SDL_Texture* NewTexture(SDL_Renderer* renderer, int w, int h, bool transparentAble, bool forCairo)
{
	const auto newTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		forCairo ? SDL_TEXTUREACCESS_STREAMING : SDL_TEXTUREACCESS_TARGET,
		w,
		h
	);

	if (!newTexture)
	{
		const auto creationError = SDL_GetError();
		ReportError("Failed to create texture", creationError);
	}

	if (transparentAble)
	{
		if (SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND) < 0)
		{
			const auto blendModeError = SDL_GetError();
			ReportError("Failed to set texture blend mode", blendModeError);
			SDL_DestroyTexture(newTexture);
			
			return NULL;
		}
	}

	return newTexture;
}

cairo_t* BeginTextureDrawing(SDL_Texture* targetTexture, const int canvasWidth, const int canvasHeight)
{
	int pitch;
	void* pixels;

	if (targetTexture)
	{
		SDL_LockTexture(targetTexture, NULL, &pixels, &pitch);
		const auto cairoSurface = cairo_image_surface_create_for_data(
			static_cast<unsigned char*>(pixels),
			CAIRO_FORMAT_ARGB32,
			canvasWidth,
			canvasHeight,
			pitch
		);

		const auto sink = cairo_create(cairoSurface);
		cairo_set_antialias(sink, CAIRO_ANTIALIAS_SUBPIXEL);
		
		return sink;
	}

	return NULL;
}

SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight)
{
	int pitch;
	void* pixels;

	const auto targetTexture = SDL_CreateTexture(
		renderTarget,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		canvasWidth,
		canvasHeight
	);

	if (targetTexture)
	{
		SDL_SetTextureBlendMode(targetTexture, SDL_BLENDMODE_BLEND);
		SDL_LockTexture(targetTexture, NULL, &pixels, &pitch);
		const auto cairoSurface = cairo_image_surface_create_for_data(
			static_cast<unsigned char*>(pixels),
			CAIRO_FORMAT_ARGB32,
			canvasWidth,
			canvasHeight,
			pitch
		);

		drawingSink = cairo_create(cairoSurface);
		cairo_set_antialias(drawingSink, CAIRO_ANTIALIAS_SUBPIXEL);
	}

	return targetTexture;
}

cairo_t* GetDrawingSink()
{
	return drawingSink;
}

void EndRenderDrawing(SDL_Renderer* renderTarget, SDL_Texture* targetTexture, SDL_Rect* destRect)
{
	SDL_UnlockTexture(targetTexture);
	SDL_SetTextureBlendMode(targetTexture, SDL_BLENDMODE_BLEND);
	SDL_RenderCopy(renderTarget, targetTexture, NULL, destRect);

	SDL_DestroyTexture(targetTexture);
	cairo_destroy(drawingSink);
}

void EndTextureDrawing(SDL_Texture* targetTexture, cairo_t* drawer)
{
	SDL_UnlockTexture(targetTexture);
	cairo_destroy(drawer);
}
