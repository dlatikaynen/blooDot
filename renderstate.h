#pragma once
#include <SDL.h>
#include <iostream>
#include <cairo.h>
#include "enums.h"
#include "gamestate.h"
#include "dingsheet.h"

/* what else would the almighty bunghole be? */
constexpr unsigned short BUNGHOLE = 4;

extern void ReportError(const char*, const char*);
extern cairo_t* BeginTextureDrawing(SDL_Texture*);
extern void EndTextureDrawing(SDL_Texture*, cairo_t*);
extern void PopulateFlap(int flapIndex);
extern WorldPiece GetPieceRelative(int, int);

bool InitializeAllFlaps(int width, int height);
void RecomputeFlapConstellation();
void Scroll(int dx, int dy);
void PopulateAllFlaps();
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

SDL_Texture* _NewTexture(SDL_Renderer* renderer, bool transparentAble, bool forCairo = false);
void _SnapHorizontal(int dx);
void _SnapVertical(int dy);
void _Unsnap(int dx, int dy);
