#pragma once
#include <cairo.h>
#include <SDL.h>


cairo_t* BeginTextureDrawing(SDL_Texture* targetTexture);
SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight);
cairo_t* GetDrawingSink();
void EndTextureDrawing(SDL_Texture* targetTexture, cairo_t* drawer);
void EndRenderDrawing(SDL_Renderer* renderTarget, SDL_Texture* targetTexture);
