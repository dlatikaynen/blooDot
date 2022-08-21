#include "pch.h"
#include "geom2d.h"
#include "gamestate.h"
#include <algorithm>

SDL_Rect GetPolyBoundingBox(std::vector<PointInWorld>* polygonRegion)
{
    int minX = 0;
    int maxX = 0;
    int minY = 0;
    int maxY = 0;

    for (auto& point : *polygonRegion)
    {
        minX = std::min(point.x, minX);
        maxX = std::max(point.x, maxX);
        minY = std::min(point.y, minY);
        maxY = std::max(point.y, maxY);
    }

    return { minX, minY, maxX - minX, maxY - minY };
}

/// <summary>
/// Jordan's theorem, not optimized for the convex case
/// Inspired by samples found at https://stackoverflow.com/q/11716268/1132334
/// </summary>
bool WorldCoordinateInRegion(const std::vector<PointInWorld>* polygonRegion, int x, int y)
{
    const auto& polyRef = *polygonRegion;
    const auto numVertices = polyRef.size();
    if (numVertices < 3)
    {
        /* a one-dimensional object is not a continuous
         * simple closed curve, so we default to "outside" */
        return false;
    }

    bool inside = false;
    for (auto i = 0; i < numVertices; ++i)
    {
        const auto j = (i + 1) % numVertices;
        const auto x0 = polyRef[i].x;
        const auto y0 = polyRef[i].y;
        const auto x1 = polyRef[j].x;
        const auto y1 = polyRef[j].y;

        if ((y0 <= y) && (y1 > y) || (y1 <= y) && (y0 > y))
        {
            const auto intersection = (x1 - x0) * (y - y0) / (y1 - y0) + x0;
            if (intersection < x)
            {
                inside = !inside;
            }
        }
    }

    return inside;
}

bool PointInRect(int x, int y, SDL_Rect r)
{
    const auto x1 = r.x + r.w;
    const auto y1 = r.y + r.h;

    // bitwise is intentional here
    return ((r.x < x) && (x < x1)) & ((r.y < y) && (y < y1));
}

bool PointInRect(int x, int y, int x0, int y0, int x1, int y1)
{
    // bitwise is intentional here, https://stackoverflow.com/a/13685281/1132334
    return ((x0 < x) && (x < x1)) & ((y0 < y) && (y < y1));
}