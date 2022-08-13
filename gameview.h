#pragma once
#include <SDL.h>

extern bool InitializeAllFlaps(int width, int height);
extern void PopulateAllFlaps();
extern void Scroll(int dx, int dy);
extern void RenderFloorBung();
extern void RenderFloorVert();
extern void RenderFloorHorz();
extern void RenderFloorQuart();
extern void RenderMobs();
extern void RenderWallsAndRooofBung();
extern void RenderstateTeardown();
extern void RenderWallsAndRooofVert();
extern void RenderWallsAndRooofHorz();
extern void RenderWallsAndRooofQuart();

bool GameviewEnterWorld();
void GameViewRenderFrame();
void GameviewTeardown();