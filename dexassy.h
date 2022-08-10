#pragma once
#include <SDL.h>

constexpr auto CHUNK_KEY_SPLASH = 0;
constexpr auto CHUNK_KEY_FONT_TITLE = 1;
constexpr auto CHUNK_KEY_FONT_DIALOG = 2;
constexpr auto CHUNK_KEY_FONT_ALIEN = 3;

void PrepareIndex();
bool OpenCooked();
void* Retrieve(int chunkKey, __out SDL_RWops** const stream);
void CloseCooked();