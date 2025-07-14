#ifndef DRAWING_H
#define DRAWING_H

#include "../../cairo/include/cairo.h"
#include <SDL3/SDL.h>

namespace blooDot::Drawing {
    SDL_Texture* NewTexture(SDL_Renderer* renderer, int w, int h, bool transparentAble, bool forCairo = false);
    cairo_t* BeginTextureDrawing(SDL_Texture* targetTexture, int canvasWidth, int canvasHeight);
    void EndTextureDrawing(SDL_Texture* targetTexture, cairo_t* drawer);
    SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, int canvasWidth, int canvasHeight);
    void EndRenderDrawing(SDL_Renderer* renderTarget, SDL_Texture* targetTexture, const SDL_FRect* destRect);
    cairo_t* GetDrawingSink();
    cairo_t* SetSourceColor(cairo_t* context, SDL_Color color);
    void DestroyTexture(SDL_Texture** texture);
}
#endif //DRAWING_H
