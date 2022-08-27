#pragma once
#include <SDL.h>
#include "chunk-constants.h"

extern void ReportError(const char*, const char*);

void PrepareIndex();
bool OpenCooked();
void* Retrieve(int chunkKey, __out SDL_RWops** const stream);
void CloseCooked();