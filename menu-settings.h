#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cairo.h>
#include "splash.h"

extern bool ScreenSettingsMenuLoop(SDL_Renderer*);

bool SettingsMenuLoop(SDL_Renderer*);

bool _EnterAndHandleSettingsMenu(SDL_Renderer* renderer);
void _EnterAndHandleScreenSettings(SDL_Renderer* renderer);