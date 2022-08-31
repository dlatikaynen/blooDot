#pragma once
#include <SDL.h>
#include <iostream>

bool GpuChanLoop(SDL_Renderer*, const char* title, std::string bubble);

void _Enter(SDL_Renderer* renderer);
void _PullOut();
