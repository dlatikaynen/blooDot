#ifndef MENU_SETTINGS_H
#define MENU_SETTINGS_H

#include <SDL3/SDL.h>

namespace blooDot::MenuSettings
{
    bool MenuLoop(SDL_Renderer*);

    void PrepareTextInternal(SDL_Renderer* renderer, bool destroy = false);
    bool EnterAndHandleSettingsMenuInternal(SDL_Renderer* renderer);
    void EnterAndHandleScreenSettingsInternal(SDL_Renderer* renderer);
    bool EnterAndHandleLanguageSettingsInternal(SDL_Renderer* renderer);
    void EnterAndHandleControlsSettingsInternal(SDL_Renderer* renderer);
    bool EnterAndHandleAboutInternal(SDL_Renderer* renderer);
    void EnterAndHandleHelpInternal(SDL_Renderer* renderer);
}
#endif //MENU_SETTINGS_H
