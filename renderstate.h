#pragma once
#include <SDL.h>
#include <cairo.h>

extern void ReportError(const char*, const char*);
extern cairo_t* BeginTextureDrawing(SDL_Texture*);
extern void EndTextureDrawing(SDL_Texture*, cairo_t*);

bool InitializeAllFlaps(int width, int height);
void PopulateAllFlaps();
void PopulateFlap(int flapIndex);
void RenderFloor();
void RenderMobs();
void RenderWalls();
void RenderRooof();
void RenderstateTeardown();