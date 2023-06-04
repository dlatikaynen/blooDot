#pragma once
#include <SDL_ttf.h>

constexpr auto FONT_KEY_DIALOG = 0;
constexpr auto FONT_KEY_DIALOG_FAT = 1;
constexpr auto FONT_KEY_FANCY = 2;
constexpr auto FONT_KEY_ALIEN = 3;

extern void ReportError(const char*, const char*);
extern void* Retrieve(int chunkKey, __out SDL_RWops** const stream);

bool LoadFonts();
TTF_Font* GetFont(int fontKey);
SDL_Texture* RenderText(SDL_Renderer* renderer, SDL_Rect *frame, int fontKey, int sizePt, const char* text, SDL_Color color, bool bold = false);
void CloseFonts();

void* LoadFontInternal(int, __out TTF_Font**, __out SDL_RWops**);