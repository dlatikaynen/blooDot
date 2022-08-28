#pragma once
#include <SDL.h>
#include "chunk-constants.h"

extern void ReportError(const char*, const char*);
extern void* HuffDeflate(SDL_RWops* inFile, long long const sourceSize, long long* originalSize, long long* actuallyRead);

void PrepareIndex();
bool OpenCooked();
void* Retrieve(int chunkKey, __out SDL_RWops** const stream);
void CloseCooked();