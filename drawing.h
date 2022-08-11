#pragma once
#include <cairo.h>
#include <SDL.h>

SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget);
cairo_t* GetDrawingSink();
void EndRenderDrawing(SDL_Renderer* renderTarget, SDL_Texture* targetTexture);
