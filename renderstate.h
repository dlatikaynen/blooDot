#pragma once
#include <SDL.h>
#include <iostream>
#include <cairo.h>
#include "enums.h"
#include "gamestate.h"
#include "dingsheet.h"

/* what else would the almighty bunghole be? */
constexpr unsigned short BUNGHOLE = 4;

constexpr unsigned short flapIndicesHorz[] = { 3,4,5 };
constexpr unsigned short flapIndicesVert[] = { 1,4,7 };
constexpr int const flapWorldOffsetsX[9] = { -1,0,1,-1,0,1,-1,0,1 };
constexpr int const flapWorldOffsetsY[9] = { -1,-1,-1,0,0,0,1,1,1 };

extern void ReportError(const char*, const char*);
extern cairo_t* BeginTextureDrawing(SDL_Texture*);
extern void EndTextureDrawing(SDL_Texture*, cairo_t*);
extern void PopulateFlap(int, int, int);
extern WorldPiece GetPieceRelative(int, int);
extern int FlapAwarenessLeft();
extern int FlapAwarenessRite();
extern int FlapAwarenessUuup();
extern int FlapAwarenessDown();

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
void _FlapoverLeft();
void _FlapoverRite();
void _FlapoverDown();
void _FlapoverUuup();
