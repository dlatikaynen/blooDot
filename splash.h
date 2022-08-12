#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cairo.h>

extern void ReportError(const char*, const char*);
extern void* Retrieve(int chunkKey, __out SDL_RWops** const stream);
extern bool LoadFonts();
extern TTF_Font* GetFont(int fontKey);
extern SDL_Texture* RenderText(SDL_Renderer*, SDL_Rect*, int, int, const char*, SDL_Color);
extern SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget);
extern cairo_t* GetDrawingSink();
extern void EndRenderDrawing(SDL_Renderer* renderTarget, SDL_Texture* targetTexture);
extern cairo_t* DrawButton(cairo_t*);
extern void DrawLabel(SDL_Renderer*, int, int, SDL_Texture*, SDL_Rect*);
extern void LoadSettings();

void SplashLoop(SDL_Renderer*);
void Bounce(SDL_Rect* srcRect);