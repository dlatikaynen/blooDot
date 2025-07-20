#include "../../main.h"
#include "../../xassy/dexassy.h"
#include "../../res/chunk-constants.h"
#include "sfx.h"
#include <map>
#include <ranges>
#include <sstream>

namespace blooDot::Sfx
{
	std::map<SoundEffect, Mix_Chunk*> sfx;

	void PreloadMenuSfx()
	{
		PreloadSingleInternal(SFX_BUMP);
		PreloadSingleInternal(SFX_SELCHG);
		PreloadSingleInternal(SFX_SELCONF);
		PreloadSingleInternal(SFX_ASTERISK);
		PreloadSingleInternal(SFX_CRICKET_FRITZ);
		PreloadSingleInternal(SFX_CRICKET_FRANZ);
		PreloadSingleInternal(SFX_FLOOR);
		PreloadSingleInternal(SFX_WALLS);
		PreloadSingleInternal(SFX_ROOOF);
	}

	void PreloadGameSfx()
	{
		PreloadSingleInternal(SFX_BULLET_DECAY);
	}

	void Play(const SoundEffect effect)
	{
		auto cacheEntry = sfx[effect];
		if (!cacheEntry)
		{
			cacheEntry = LoadSingleInternal(effect);
		}

		if (cacheEntry)
		{
#ifndef NDEBUG
			const auto result = Mix_PlayChannel(-1, cacheEntry, 0);

			if (result < 0)
			{
				const auto& playError = SDL_GetError();

				ReportError("FAIL Could not play sound effect", playError);

				return;
			}

			std::stringstream whichEffect;
			whichEffect << "INFO Played " << effect << " on channel #" << result << "\n";
			std::cout << whichEffect.str();
#else
			Mix_PlayChannel(-1, cacheEntry, 0);
#endif
		}
	}

	void Teardown()
	{
		for (const auto &val: sfx | std::views::values)
		{
			if (val)
			{
				Mix_FreeChunk(val);
			}
		}

		sfx.clear();
	}

	void PreloadSingleInternal(const SoundEffect& effect)
	{
		if (auto eff = LoadSingleInternal(effect))
		{
			sfx.emplace(effect, eff);
		}
	}

	Mix_Chunk* LoadSingleInternal(const SoundEffect& effect)
	{
		const auto& chunkKey = GetResourceKeyInternal(effect);
		SDL_IOStream* soundFile;
		const auto soundMem = Retrieve(chunkKey, &soundFile);
		const auto& soundEffect = Mix_LoadWAV_IO(soundFile, false);

		if (!soundEffect)
		{
			const auto loadError = SDL_GetError();

			std::stringstream loadMessage;
			loadMessage << "FAIL Could not load the " << effect << " sound effect";

			const auto& strLoadMessage = loadMessage.str();

			ReportError(strLoadMessage.c_str(), loadError);

			return nullptr;
		}

		if (!SDL_CloseIO(soundFile)) {
			const auto closeError = SDL_GetError();

			std::stringstream closeMessage;
			closeMessage << "WARN Failed to close the stream after loading the " << effect << " sound effect";

			const auto& strCloseMessage = closeMessage.str();

			ReportError(strCloseMessage.c_str(), closeError);
		}

		SDL_free(soundMem);

		return soundEffect;
	}

	int GetResourceKeyInternal(const SoundEffect& effect)
	{
		switch (effect)
		{
		case SFX_BULLET_DECAY:
			return CHUNK_KEY_SFX_PROJECTILEDECAY;

		case SFX_BUMP:
			return CHUNK_KEY_SFX_BUMP;

		case SFX_SELCHG:
			return CHUNK_KEY_SFX_SELCHG;

		case SFX_SELCONF:
			return CHUNK_KEY_SFX_SELCONF;

		case SFX_ASTERISK:
			return CHUNK_KEY_SFX_ASTERISK;

		case SFX_CRICKET_FRANZ:
			return CHUNK_KEY_SFX_CRICKET_FRANZ;

		case SFX_CRICKET_FRITZ:
			return CHUNK_KEY_SFX_CRICKET_FRITZ;

		case SFX_FLOOR:
			return CHUNK_KEY_SFX_FLOOR;

		case SFX_WALLS:
			return CHUNK_KEY_SFX_WALLS;

		case SFX_ROOOF:
			return CHUNK_KEY_SFX_ROOOF;

		default:
			throw;
		}
	}
}
