#ifndef MENU_SETTINGS_DISPLAYS_H
#define MENU_SETTINGS_DISPLAYS_H
#pragma once
#include <SDL3/SDL.h>

namespace blooDot::Constants {
    enum ViewportResolutions : char;
}

namespace blooDot::MenuSettingsDisplays
{
    bool ScreenSettingsMenuLoop(SDL_Renderer*);

    void PrepareControlsInternal(SDL_Renderer* renderer);
    void AnimateCarouselInternal();
    void TeardownInternal();
    bool CanSelectModeInternal(SDL_Renderer* renderer);
    bool GetResolutionInternal(Constants::ViewportResolutions videoMode, SDL_Rect* dimensions);
}
#endif //MENU_SETTINGS_DISPLAYS_H
