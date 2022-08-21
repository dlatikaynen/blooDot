#pragma once
#include <stdlib.h>
#include "gamestate.h"

SDL_Rect GetPolyBoundingBox(std::vector<PointInWorld>* polygonRegion);
bool WorldCoordinateInRegion(std::vector<PointInWorld> polygon, int x, int y);
