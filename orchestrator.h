#pragma once
#include <SDL2/SDL.h>
#include <box2d/box2d.h>

extern bool InitializeNewWorld();
extern bool GameviewEnterWorld();
extern void EnsurePlayers();
extern void GameViewRenderFrame();
extern void NudgePlayer(int playerIndex, int accelerationX, int accelerationY);
extern void SetPlayerPosition(int playerIndex, int positionX, int positionY, int orientation);
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