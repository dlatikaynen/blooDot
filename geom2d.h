#pragma once
#include <stdlib.h>
#include "gamestate.h"

SDL_Rect GetPolyBoundingBox(std::vector<PointInWorld>* polygonRegion);
bool WorldCoordinateInRegion(const std::vector<PointInWorld>* polygonRegion, int x, int y);
bool PointInRect(int x, int y, SDL_Rect r);
bool PointInRect(int x, int y, int x0, int y0, int x1, int y1);