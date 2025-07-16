#ifndef MENU_SETTINGS_LANG_H
#define MENU_SETTINGS_LANG_H
#include <SDL3/SDL.h>

namespace blooDot::MenuSettingsLang
{
    bool LanguageSettingsMenuLoop(SDL_Renderer*);

    void PrepareControlsInternal(SDL_Renderer* renderer);
    void VignetteLabelInternal(SDL_Renderer* renderer, int font, int size, int vignetteIndex, int y, const char* text);
    void AnimateCarouselInternal();
    void TeardownInternal();
}
#endif //MENU_SETTINGS_LANG_H
