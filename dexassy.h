#pragma once
#include <SDL.h>

constexpr auto CHUNK_KEY_SPLASH = 0;
constexpr auto CHUNK_KEY_FONT_TITLE = 1;
constexpr auto CHUNK_KEY_FONT_DIALOG = 2;
constexpr auto CHUNK_KEY_FONT_ALIEN = 3;
constexpr auto CHUNK_KEY_SFX_PROJECTILEDECAY = 4;
constexpr auto CHUNK_KEY_DINGS_BARREL_INDIGO = 5;
constexpr auto CHUNK_KEY_DINGS_BARREL_WOODEN = 6;
constexpr auto CHUNK_KEY_DINGS_CHEL_F = 7;
constexpr auto CHUNK_KEY_DINGS_FLOORROCKTILE_CRACKED = 8;
constexpr auto CHUNK_KEY_DINGS_FLOORROCKTILE = 9;
constexpr auto CHUNK_KEY_DINGS_FLOORSTONETILE_OCHRE = 10;
constexpr auto CHUNK_KEY_DINGS_FLOORSTONETILE_PURPLE = 11;
constexpr auto CHUNK_KEY_DINGS_FLOORSTONETILE_SLATE = 12;
constexpr auto CHUNK_KEY_DINGS_GRASS = 13;
constexpr auto CHUNK_KEY_DINGS_LETTUCE = 14;
constexpr auto CHUNK_KEY_DINGS_SCHAED = 15;
constexpr auto CHUNK_KEY_DINGS_SNOW = 16;

extern void ReportError(const char*, const char*);

void PrepareIndex();
bool OpenCooked();
void* Retrieve(int chunkKey, __out SDL_RWops** const stream);
void CloseCooked();