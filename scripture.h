#pragma once
#include <SDL_ttf.h>

constexpr auto FONT_KEY_TITLE = 0;
constexpr auto FONT_KEY_DIALOG = 1;
constexpr auto FONT_KEY_ALIEN = 2;

bool LoadFonts();
TTF_Font* GetFont(int fontKey);
void CloseFonts();
