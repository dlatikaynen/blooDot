#include "pch.h"
#include "sfx.h"
#include "dexassy.h"
#include <iostream>

namespace blooDot::Sfx
{
	std::map<SoundEffect, Mix_Chunk*> sfx;

	void PreloadMenuSfx()
	{
		_PreloadSingle(SoundEffect::SFX_BUMP);
		_PreloadSingle(SoundEffect::SFX_SELCHG);
		_PreloadSingle(SoundEffect::SFX_SELCONF);
		_PreloadSingle(SoundEffect::SFX_ASTERISK);
		_PreloadSingle(SoundEffect::SFX_CRICKET_FRITZ);
		_PreloadSingle(SoundEffect::SFX_CRICKET_FRANZ);
		_PreloadSingle(SoundEffect::SFX_FLOOR);
		_PreloadSingle(SoundEffect::SFX_WALLS);
		_PreloadSingle(SoundEffect::SFX_ROOOF);
	}

	void PreloadGameSfx()
	{
		_PreloadSingle(SoundEffect::SFX_BULLET_DECAY);
	}

	void Play(const SoundEffect effect)
	{
		auto cacheEntry = sfx[effect];
		if (!cacheEntry)
		{
			cacheEntry = _LoadSingle(effect);
		}

		if (cacheEntry)
		{
#ifndef NDEBUG
			const auto result = Mix_PlayChannel(-1, cacheEntry, 0);
			if (result < 0)
			{
				const auto& playError = Mix_GetError();
				ReportError("Failed to play sound effect", playError);
				return;
			}

			std::stringstream whichEffect;
			whichEffect << "Played " << effect << " on channel #" << result << "\n";
			std::cout << whichEffect.str();
#else
			Mix_PlayChannel(-1, cacheEntry, 0);
#endif
		}
	}

	void Teardown()
	{
		for (const auto& effect : sfx)
		{
			if (effect.second)
			{
				Mix_FreeChunk(effect.second);
			}
		}

		sfx.clear();
	}

	void _PreloadSingle(const SoundEffect& effect)
	{
		auto eff = _LoadSingle(effect);
		if (eff)
		{
			sfx.emplace(effect, eff);
		}
	}

	Mix_Chunk* _LoadSingle(SoundEffect effect)
	{
		const auto& chunkKey = _GetResourceKey(effect);
		SDL_RWops* soundFile;
		const auto soundMem = Retrieve(chunkKey, &soundFile);
		const auto& soundEffect = Mix_LoadWAV_RW(soundFile, -1);
		if (!soundEffect)
		{
			const auto loadError = SDL_GetError();
			std::stringstream loadMessage;
			loadMessage << "Failed to load the " << effect << " sound effect";
			const auto& strLoadMessage = loadMessage.str();

			ReportError(strLoadMessage.c_str(), loadError);
		}

		//soundFile->close(soundFile);
		SDL_free(soundMem);

		return soundEffect;
	}

	int _GetResourceKey(SoundEffect effect)
	{
		switch (effect)
		{
		case SoundEffect::SFX_BULLET_DECAY:
			return CHUNK_KEY_SFX_PROJECTILEDECAY;

		case SoundEffect::SFX_BUMP:
			return CHUNK_KEY_SFX_BUMP;
		
		case SoundEffect::SFX_SELCHG:
			return CHUNK_KEY_SFX_SELCHG;

		case SoundEffect::SFX_SELCONF:
			return CHUNK_KEY_SFX_SELCONF;

		case SoundEffect::SFX_ASTERISK:
			return CHUNK_KEY_SFX_ASTERISK;

		case SoundEffect::SFX_CRICKET_FRANZ:
			return CHUNK_KEY_SFX_CRICKET_FRANZ;

		case SoundEffect::SFX_CRICKET_FRITZ:
			return CHUNK_KEY_SFX_CRICKET_FRITZ;

		case SoundEffect::SFX_FLOOR:
			return CHUNK_KEY_SFX_FLOOR;

		case SoundEffect::SFX_WALLS:
			return CHUNK_KEY_SFX_WALLS;

		case SoundEffect::SFX_ROOOF:
			return CHUNK_KEY_SFX_ROOOF;

		default:
			throw;
		}
	}
}