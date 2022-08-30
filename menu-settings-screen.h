#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cairo.h>
#include "splash.h"
#include "boydsa.h"

extern SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight);

bool ScreenSettingsMenuLoop(SDL_Renderer*);

void _PrepareControls(SDL_Renderer* renderer);
void _VignetteLabel(SDL_Renderer* renderer, int font, int size, int vignetteIndex, int y, const char* text);
void _AnimateCarousel();
void _TeardownScreenSettingsMenu();
bool _CanSelectMode();