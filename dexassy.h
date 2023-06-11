#pragma once
#include <SDL2/SDL.h>
#include "chunk-constants.h"

constexpr char const* XassyFile = "xassy-cooked.1.ngld";

extern void ReportError(const char*, const char*);
extern void* HuffInflate(SDL_RWops* inFile, long long const sourceSize, long long* originalSize, long long* actuallyRead);

void PrepareIndex();
bool OpenCooked();
void* Retrieve(int chunkKey, __out SDL_RWops** const stream);
void CloseCooked();