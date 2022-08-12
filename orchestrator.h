#pragma once
#include <SDL.h>

extern bool GameviewEnterWorld();
extern void GameViewRenderFrame();
extern void GameviewTeardown();

void MainLoop(SDL_Renderer*);