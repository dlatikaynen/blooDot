#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cairo.h>

namespace blooDot::MenuInGame
{
	bool MenuLoop(SDL_Renderer*);

	void _HandleMenu();
	void _HandleSave();
}