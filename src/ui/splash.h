#ifndef SPLASH_H
#define SPLASH_H

#include <SDL3/SDL.h>
#include "menu-common.h"

namespace blooDot::Splash
{
    MenuCommon::MenuDialogInteraction SplashLoop(SDL_Renderer*);

    bool LoadSplashInternal(SDL_Renderer* renderer);
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
