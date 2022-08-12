#pragma once
#include <SDL.h>
#include <cairo.h>

cairo_t* DrawButton(cairo_t* context, double x, double y, double w, double h);
void DrawLabel(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_Rect* frame);
