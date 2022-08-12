#pragma once
#include <SDL.h>

extern bool InitializeAllFlaps(int width, int height);
extern void PopulateAllFlaps();
extern void RenderFloor();
extern void RenderMobs();
extern void RenderWalls();
extern void RenderRooof();
extern void RenderstateTeardown();

bool GameviewEnterWorld();
void GameViewRenderFrame();
void GameviewTeardown();