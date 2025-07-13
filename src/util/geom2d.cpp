#include "geom2d.h"

namespace blooDot::Geometry2d
{
    void CenterRectInRect(const SDL_FRect* container, const SDL_FRect* candidate, SDL_FRect* centered)
    {
        auto&[x, y, w, h] = *centered;

        x = container->x + (container->w - candidate->w) / 2;
        y = container->y + (container->h - candidate->h) / 2;
        w = candidate->w;
        h = candidate->h;
    }
}