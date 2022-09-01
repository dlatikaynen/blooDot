#include "pch.h"
#include "drawing.h"
#include "constants.h"

cairo_t* drawingSink = NULL;

cairo_t* BeginTextureDrawing(SDL_Texture* targetTexture)
{
	int width = GodsPreferredWidth;
	int height = GodsPreferredHight;
	int pitch;
	void* pixels;

	if (targetTexture)
	{
		SDL_LockTexture(targetTexture, NULL, &pixels, &pitch);
		const auto cairoSurface = cairo_image_surface_create_for_data(
			static_cast<unsigned char*>(pixels),
			CAIRO_FORMAT_ARGB32,
			width,
			height,
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

void EndRenderDrawing(SDL_Renderer* renderTarget, SDL_Texture* targetTexture)
{
	SDL_UnlockTexture(targetTexture);
	SDL_SetTextureBlendMode(targetTexture, SDL_BLENDMODE_BLEND);
	SDL_RenderCopy(renderTarget, targetTexture, NULL, NULL);

	SDL_DestroyTexture(targetTexture);
	cairo_destroy(drawingSink);
}

void EndTextureDrawing(SDL_Texture* targetTexture, cairo_t* drawer)
{
	SDL_UnlockTexture(targetTexture);
	cairo_destroy(drawer);
}
