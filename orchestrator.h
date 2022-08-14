#pragma once
#include <SDL.h>

extern bool InitializeNewWorld();
extern bool GameviewEnterWorld();
extern void GameViewRenderFrame();
extern void Scroll(int, int);
extern void TeardownDingSheets();
extern void GameviewTeardown();
extern void ClearWorldData();

void MainLoop(SDL_Renderer*);