#pragma once
#include <cairo.h>
#include <SDL.h>

cairo_t* RenderDrawing(SDL_Renderer* renderTarget, cairo_t* (*drawJob)(cairo_t*));
