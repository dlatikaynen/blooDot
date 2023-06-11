#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "enums.h"
#include "chunk-constants.h"

namespace blooDot::Sfx
{
	void PreloadMenuSfx();
	void PreloadGameSfx();
	void Play(const SoundEffect sfx);
	void Teardown();

	void _PreloadSingle(const SoundEffect& effect);
	Mix_Chunk* _LoadSingle(const SoundEffect effect);
	int _GetResourceKey(const SoundEffect effect);
}