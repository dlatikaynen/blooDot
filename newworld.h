#pragma once
#include <SDL.h>
#include "gamestate.h"

extern void ReportError(const char*, const char*);

bool InitializeNewWorld();

void _Put(WorldSheet* sheet, int x, int y, Ding ding, DingProps props = DingProps::Default);