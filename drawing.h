#pragma once
#include <cairo.h>
#include <SDL.h>

extern void ReportError(const char*, const char*);

SDL_Texture* NewTexture(SDL_Renderer* renderer, int w, int h, bool transparentAble, bool forCairo = false);
cairo_t* BeginTextureDrawing(SDL_Texture* targetTexture, const int canvasWidth, const int canvasHeight);
SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight);
cairo_t* GetDrawingSink();
void EndTextureDrawing(SDL_Texture* targetTexture, cairo_t* drawer);
void EndRenderDrawing(SDL_Renderer* renderTarget, SDL_Texture* targetTexture);
