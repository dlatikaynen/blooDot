#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include "dexassy.h"
#include <SDL2/SDL_image.h>
#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include "constants.h"
#include "resutil.h"

bool GpuChanLoop(SDL_Renderer* renderer, const char* message, const char* title, std::string bubble);

void _Enter(SDL_Renderer* renderer);
void _PullOut();
