#ifndef SCRIPTURE_H
#define SCRIPTURE_H

#include <SDL3/SDL.h>
#include "SDL3_ttf/SDL_ttf.h"

namespace blooDot::Scripture {
    constexpr auto FONT_KEY_DIALOG = 0;
    constexpr auto FONT_KEY_DIALOG_FAT = 1;
    constexpr auto FONT_KEY_FANCY = 2;
    constexpr auto FONT_KEY_ALIEN = 3;

    bool LoadFonts();
    TTF_Font* GetFont(int fontKey);
    bool DrawText(SDL_Texture* streamingDestination, SDL_Rect* frame, int fontKey, int sizePt, const char* text, SDL_Color color, bool bold = false);
    SDL_Texture* RenderText(SDL_Renderer* renderer, SDL_FRect *frame, int fontKey, int sizePt, const char* text, SDL_Color color, bool bold = false);
    void CloseFonts();

    void* LoadFontInternal(int, TTF_Font**, SDL_IOStream**);
}
#endif //SCRIPTURE_H
