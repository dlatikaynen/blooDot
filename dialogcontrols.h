#pragma once
#include "enums.h"
#include <SDL2/SDL.h>
#include <cairo/cairo.h>

cairo_t* DrawButton(cairo_t* context, double x, double y, double w, double h, ControlHighlight highlight);
void DrawLabel(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_Rect* frame);
void CenterLabel(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_Rect* frame);
cairo_t* DrawChevron(cairo_t* context, double centerx, double centery, bool left_or_right, unsigned short nudge);
void TeardownDialogControls();

void _PrepareSelPatterns();