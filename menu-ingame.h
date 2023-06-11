#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>

namespace blooDot::MenuInGame
{
	bool MenuLoop(SDL_Renderer*);

	void _HandleMenu();
	void _HandleSave();
	void _HandleLeave();
}