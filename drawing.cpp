#include "pch.h"
#include "drawing.h"

cairo_t* RenderDrawing(SDL_Renderer* renderTarget, cairo_t* (*drawJob)(cairo_t*))
{
	int width = 640;
	int height = 480;
	int pitch;
	void* pixels;

	const auto targetTexture = SDL_CreateTexture(
		renderTarget,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		width,
		height
	);

	SDL_LockTexture(targetTexture, NULL, &pixels, &pitch);
	const auto cairoSurface = cairo_image_surface_create_for_data(
		static_cast<unsigned char*>(pixels),
		CAIRO_FORMAT_ARGB32,
		width,
		height,
		pitch
	);

	const auto drawingSink = cairo_create(cairoSurface);
	const auto result = drawJob(drawingSink);

	SDL_UnlockTexture(targetTexture);
	SDL_SetTextureBlendMode(targetTexture, SDL_BLENDMODE_BLEND);
	SDL_RenderCopy(renderTarget, targetTexture, NULL, NULL);

	return result;
}
