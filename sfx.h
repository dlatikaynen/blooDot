#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include "enums.h"
#include "chunk-constants.h"

namespace blooDot::Sfx
{
	void PreloadMenuSfx();
	void PreloadGameSfx();
	void Teardown();

	Mix_Chunk* _LoadSingle(SoundEffect effect);
	int _GetResourceKey(SoundEffect effect);
}