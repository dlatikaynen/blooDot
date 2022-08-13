#pragma once
#include <SDL.h>
#include <cairo.h>
#include "enums.h"

/* what else would the almighty bunghole be? */
constexpr unsigned short BUNGHOLE = 4;

extern void ReportError(const char*, const char*);
extern cairo_t* BeginTextureDrawing(SDL_Texture*);
extern void EndTextureDrawing(SDL_Texture*, cairo_t*);

bool InitializeAllFlaps(int width, int height);
void RecomputeFlapConstellation();
void RecomputeFlapRects();
void PopulateAllFlaps();
void PopulateFlap(int flapIndex);
void RenderFloorBung();
void RenderFloorVert();
void RenderFloorHorz();
void RenderFloorQuart();
void RenderMobs();
void RenderWallsAndRooofBung();
void RenderWallsAndRooofVert();
void RenderWallsAndRooofHorz();
void RenderWallsAndRooofQuart();
void RenderstateTeardown();