#include "drawing.h"
#include "../../main.h"

namespace blooDot::Drawing {
	cairo_t* drawingSink = nullptr;

	SDL_Texture* NewTexture(SDL_Renderer* renderer, const int w, const int h, const bool transparentAble, const bool forCairo)
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

				return nullptr;
			}
		}

		return newTexture;
	}

	cairo_t* BeginTextureDrawing(SDL_Texture* targetTexture, const int canvasWidth, const int canvasHeight)
	{
		int stride;
		void* pixels;

		if (targetTexture)
		{
			SDL_LockTexture(targetTexture, nullptr, &pixels, &stride);
			const auto cairoSurface = cairo_image_surface_create_for_data(
				static_cast<unsigned char*>(pixels),
				CAIRO_FORMAT_ARGB32,
				canvasWidth,
				canvasHeight,
				stride
			);

			const auto sink = cairo_create(cairoSurface);
			cairo_set_antialias(sink, CAIRO_ANTIALIAS_SUBPIXEL);

			return sink;
		}

		return nullptr;
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
			SDL_LockTexture(targetTexture, nullptr, &pixels, &pitch);

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

	cairo_t* SetSourceColor(cairo_t* context, SDL_Color color)
	{
		cairo_set_source_rgba
		(
			context,
			color.r / 255.,
			color.g / 255.,
			color.b / 255.,
			color.a / 255.
		);

		return context;
	}

	/// <summary>
	/// Draws immediately on the render target (usually the backbuffer
	/// of the main window) and cleans everything up
	/// </summary>
	void EndRenderDrawing(SDL_Renderer* renderTarget, SDL_Texture* targetTexture, const SDL_FRect* destRect)
	{
		SDL_UnlockTexture(targetTexture);
		SDL_SetTextureBlendMode(targetTexture, SDL_BLENDMODE_BLEND);
		SDL_RenderTexture(renderTarget, targetTexture, nullptr, destRect);

		SDL_DestroyTexture(targetTexture);
		cairo_destroy(drawingSink);
	}

	/// <summary>
	/// Only cleans up cairo, does not touch the finished texture
	/// </summary>
	void EndTextureDrawing(SDL_Texture* targetTexture, cairo_t* drawer)
	{
		SDL_UnlockTexture(targetTexture);
		cairo_destroy(drawer);
	}

	void DestroyTexture(SDL_Texture** texture)
	{
		if (texture != nullptr)
		{
			if ((*texture) != nullptr)
			{
				SDL_DestroyTexture(*texture);
			}

			(*texture) = nullptr;
		}
	}
}