#pragma once
#include <SDL.h>
#include <box2d.h>

extern bool InitializeNewWorld();
extern bool GameviewEnterWorld();
extern void AttachWorldPhysics(b2World* world, int flapOffsetX, int flapOffsetY);
extern void EnsurePlayers();
extern void GameViewRenderFrame();
extern void NudgePlayer(int playerIndex, int accelerationX, int accelerationY);
extern void SetPlayerPosition(int playerIndex, int positionX, int positionY);
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