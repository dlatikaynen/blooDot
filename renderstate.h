#pragma once
#include <SDL.h>

extern void ReportError(const char*, const char*);

bool InitializeAllFlaps(SDL_Renderer* renderer, int width, int height);
void PopulateAllFlaps();
void PopulateFlap(int flapIndex);
void RenderFloor();
void RenderMobs();
void RenderWalls();
void RenderRooof();
void RenderstateTeardown();