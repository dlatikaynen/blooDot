#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cairo.h>

extern void ReportError(const char*, const char*);
extern void* Retrieve(int chunkKey, __out SDL_RWops** const stream);
extern bool LoadFonts();
extern TTF_Font* GetFont(int fontKey);
extern cairo_t* DrawButtonChevron(cairo_t*);

void SplashLoop(SDL_Renderer*);