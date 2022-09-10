#pragma once
#include <SDL.h>
#include <iostream>
#include "dexassy.h"
#include <SDL_image.h>
#include "scripture.h"
#include "xlations.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include "constants.h"
#include "resutil.h"

bool GpuChanLoop(SDL_Renderer* renderer, const char* message, const char* title, std::string bubble);

void _Enter(SDL_Renderer* renderer);
void _PullOut();
