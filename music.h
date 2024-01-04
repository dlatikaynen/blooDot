#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

extern void ReportError(const char*, const char*);

namespace blooDot::Music
{
	using AudioFrame = std::array<int, 8192>;

	bool StartMusic();
	void Teardown();

	void _generator();
}
