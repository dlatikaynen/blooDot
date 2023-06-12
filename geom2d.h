#pragma once
#include <stdlib.h>
#include "gamestate.h"

namespace blooDot::Geometry2d
{
	constexpr const float Rad2DegFact = 57.2957795131f;

	template <typename T> int sgn(T val) {
		return (T(0) < val) - (val < T(0));
	}

	SDL_Rect GetPolyBoundingBox(std::vector<PointInWorld>* polygonRegion);
	bool WorldCoordinateInRegion(const std::vector<PointInWorld>* polygonRegion, int x, int y);
	bool PointInRect(int x, int y, SDL_Rect r);
	bool PointInRect(int x, int y, int x0, int y0, int x1, int y1);
	void CenterRectInRect(SDL_Rect* container, SDL_Rect* candidate, __out SDL_Rect* centered);
}