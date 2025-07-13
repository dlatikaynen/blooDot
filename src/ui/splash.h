#ifndef SPLASH_H
#define SPLASH_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cairo.h>

extern void ReportError(const char*, const char*);
extern void* Retrieve(int chunkKey, SDL_IOStream** stream);
extern bool LoadFonts();
extern TTF_Font* GetFont(int fontKey);
extern SDL_Texture* RenderText(SDL_Renderer*, SDL_Rect*, int, int, const char*, SDL_Color, bool bold);
extern SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, int canvasWidth, int canvasHeight);
extern cairo_t* GetDrawingSink();
extern void EndRenderDrawing(SDL_Renderer* renderTarget, SDL_Texture* targetTexture, SDL_Rect* destRect);
extern void DrawLabel(SDL_Renderer*, int, int, SDL_Texture*, SDL_Rect*);
extern cairo_t* DrawChevron(cairo_t*, double, double, bool, unsigned short);
extern bool SettingsMenuLoop(SDL_Renderer*);

namespace blooDot::Splash
{
    bool SplashLoop(SDL_Renderer*);

    void BounceInternal(SDL_Rect* srcRect);
    void AssignNewSpeedInternal(int* speed);
    void DelayBackgroundAnimInternal();
    void PrepareTextInternal(SDL_Renderer* renderer, bool destroy = false);
    bool HandleLaunchInternal(SDL_Renderer* renderer, bool stayInMenu);
    bool EnterAndHandleMenuInternal(SDL_Renderer* renderer);
    bool EnterAndHandleCreatorModeInternal(SDL_Renderer* renderer);
    void EnterAndHandleSettingsInternal(SDL_Renderer* renderer);
}

#endif //SPLASH_H
