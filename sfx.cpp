#include "pch.h"
#include "sfx.h"
#include "dexassy.h"

namespace blooDot::Sfx
{
	std::map<SoundEffect, Mix_Chunk*> sfx;

	void PreloadMenuSfx()
	{

	}

	void PreloadGameSfx()
	{

	}

	void Play(SoundEffect effect)
	{
		auto cacheEntry = sfx[effect];
		if (!cacheEntry)
		{
			cacheEntry = _LoadSingle(effect);
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

	Mix_Chunk* _LoadSingle(SoundEffect effect)
	{
		const auto& chunkKey = _GetResourceKey(effect);
		SDL_RWops* soundFile;
		const auto soundMem = Retrieve(chunkKey, &soundFile);
		const auto& soundEffect = Mix_LoadWAV_RW(soundFile, true);
		if (!soundEffect)
		{
			const auto loadError = SDL_GetError();
			std::stringstream loadMessage;
			loadMessage << "Failed to load the " << effect << " sound effect";
			ReportError(loadMessage.str().c_str(), loadError);
		}

		soundFile->close(soundFile);
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
		
		default:
			throw;
		}
	}
}