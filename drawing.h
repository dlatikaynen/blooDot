#pragma once
#include <cairo/cairo.h>
#include <SDL2/SDL.h>

extern void ReportError(const char*, const char*);

SDL_Texture* NewTexture(SDL_Renderer* renderer, int w, int h, bool transparentAble, bool forCairo = false);
cairo_t* BeginTextureDrawing(SDL_Texture* targetTexture, const int canvasWidth, const int canvasHeight);
SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight);
cairo_t* GetDrawingSink();
cairo_t* SetSourceColor(cairo_t* context, SDL_Color color);
void EndTextureDrawing(SDL_Texture* targetTexture, cairo_t* drawer);
void EndRenderDrawing(SDL_Renderer* renderTarget, SDL_Texture* targetTexture, SDL_Rect* destRect);
void DestroyTexture(SDL_Texture** texture);