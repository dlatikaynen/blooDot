#pragma once
#include <SDL.h>
#include "gamestate.h"
#include "xlations.h"

extern void ReportError(const char*, const char*);

bool InitializeNewWorld();

void _Put(WorldSheet* sheet, int x, int y, Ding ding);