#ifndef GEOM2D_H
#define GEOM2D_H

#include "SDL3/SDL.h"

namespace blooDot::Geometry2d
{
    void CenterRectInRect(const SDL_FRect* container, const SDL_FRect* candidate, SDL_FRect* centered);
}
#endif //GEOM2D_H
