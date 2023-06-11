#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>
#include "splash.h"
#include "enums.h"
#include "sfx.h"
#include "resutil.h"
#include "geom2d.h"

extern SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight);

namespace blooDot::MultiMonitorMenuScreen
{
	typedef struct DisplayRepresentationStruct {
		SDL_Rect rect;
		int displayIndex;
	} DisplayRepresentation;

	bool MenuLoop(SDL_Renderer*);

	void _PrepareControls(SDL_Renderer* renderer);
	void _PrepareRepresentationRect(SDL_Rect* rect, int vignetteIndex);
	void _DisplayRepresentation(SDL_Renderer* renderer, SDL_Rect* dimension, int vignetteIndex, bool isCurrent);
	void _VignetteLabel(SDL_Renderer* renderer, int font, int size, int vignetteIndex, int y, const char* text);
	void _AnimateCarousel();
	void _Teardown();
}