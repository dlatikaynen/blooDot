#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>
#include "splash.h"

extern bool GpuChanLoop(SDL_Renderer* renderer, const char* message, const char* title, std::string bubble);

namespace blooDot::MenuCreatorMode
{
	bool MenuLoop(SDL_Renderer*);

	void _PrepareText(SDL_Renderer* renderer, bool destroy = false);
	bool _EnterAndHandleCreatorModeMenuItem(SDL_Renderer* renderer);
	bool _AnyTeleportTargets(SDL_Renderer* renderer);
}