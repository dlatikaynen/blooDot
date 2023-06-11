#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>
#include "splash.h"
#include "menu-settings-screen.h"
#include "menu-settings-lang.h"

extern bool ScreenSettingsMenuLoop(SDL_Renderer*);

namespace blooDot::MenuSettings
{
	bool MenuLoop(SDL_Renderer*);

	void _PrepareText(SDL_Renderer* renderer, bool destroy = false);
	bool _EnterAndHandleSettingsMenu(SDL_Renderer* renderer);
	void _EnterAndHandleScreenSettings(SDL_Renderer* renderer);
	void _EnterAndHandleLanguageSettings(SDL_Renderer* renderer);
	void _EnterAndHandleAbout(SDL_Renderer* renderer);
	void _EnterAndHandleHelp(SDL_Renderer* renderer);
}