#include "pch.h"
#include "geom2d.h"
#include "gamestate.h"

/// <summary>
/// Jordan's theorem, not optimized for the convex case
/// Inspired by samples found at https://stackoverflow.com/q/11716268/1132334
/// </summary>
bool WorldCoordinateInRegion(std::vector<PointInWorld>* polygonRegion, int x, int y)
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
#pragma warning (disable: 26451)
        const auto j = (i + 1) % numVertices;
#pragma warning (default: 26451)

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