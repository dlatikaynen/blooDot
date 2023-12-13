#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>
#include "splash.h"
#include "enums.h"

//extern SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight);

namespace blooDot::MenuSettingsControls
{

	bool ControlsSettingsMenuLoop(SDL_Renderer*);

	typedef struct ControllerStateStruct {
		bool A;
		bool B;
		bool X;
		bool Y;
		bool DL;
		bool DR;
		bool DD;
		bool DU;
		bool P1;
		bool LSH;
		bool LST;
		bool Back;
		bool Guide;
		bool Start;
		bool RST;
		bool RSH;
		bool P2;
	} ControllerState;

	void _StatusQuo();
	void _PrepareControls(SDL_Renderer* renderer);
	void _DrawController(cairo_t* drawingSink, int xMult, SDL_Color color);
	void _VignetteLabel(SDL_Renderer* renderer, int font, int size, int vignetteIndex, int y, const char* text);
	bool _RegisterButtonState(Sint32 instanceId, SDL_GameControllerButton which, bool state);
	void _AnimateCarousel();
	void _Teardown();
}