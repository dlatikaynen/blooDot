#pragma once
#include <SDL.h>

extern bool InitializeNewWorld();
extern bool GameviewEnterWorld();
extern void GameViewRenderFrame();
extern void Scroll(int, int);
extern void TeardownDingSheets();
extern void GameviewTeardown();
extern void ClearWorldData();
extern SDL_Texture* NewTexture(SDL_Renderer* renderer, int w, int h, bool transparentAble, bool forCairo);

namespace blooDot::Orchestrator
{
	void MainLoop(SDL_Renderer*);

	void _HandleSave(bool isAutosave = false);
}