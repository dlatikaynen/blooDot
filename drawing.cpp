#include "pch.h"
#include "drawing.h"

cairo_t* drawingSink = NULL;

SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget)
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

		drawingSink = cairo_create(cairoSurface);
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