#pragma once
#include <SDL2/SDL.h>
#include <box2d/box2d.h>
#include "playerhud.h"
#include "geom2d.h"
#include "newworld.h"
#include "menu-ingame.h"
#include "settings.h"
#include "savegame.h"
#include "constants.h"
#include "sfx.h"

extern bool GameviewEnterWorld();
extern void EnsurePlayers();
extern void GameViewRenderFrame();
extern void NudgePlayer(int playerIndex, int accelerationX, int accelerationY);
extern void Scroll(int, int);
extern void TeardownDingSheets();
extern void GameviewTeardown();
extern void ClearWorldData();
extern SDL_Texture* NewTexture(SDL_Renderer* renderer, int w, int h, bool transparentAble, bool forCairo);

namespace blooDot::Orchestrator
{
	void MainLoop(SDL_Renderer*);

	void _InitiatePlayerMovement(b2Body* body, float impulseX, float impulseY);
	void _HandleSave(bool isAutosave = false);
}