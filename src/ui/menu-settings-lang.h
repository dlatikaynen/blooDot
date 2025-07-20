#ifndef MENU_SETTINGS_LANG_H
#define MENU_SETTINGS_LANG_H
#include <SDL3/SDL.h>

#include "../shared-constants.h"

namespace blooDot::MenuSettingsLang
{
    Constants::DialogMenuResult LanguageSettingsMenuLoop(SDL_Renderer*);

    void PrepareControlsInternal(SDL_Renderer* renderer);
    void AnimateCarouselInternal();
    void TeardownInternal();
}
#endif //MENU_SETTINGS_LANG_H
