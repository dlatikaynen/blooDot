#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include "moosic/oscillator.h"
#include "moosic/string-shaper.h"

extern void ReportError(const char*, const char*);

namespace blooDot::Music
{
	constexpr unsigned int Frequency = 22050; // Hz
	constexpr unsigned int FrameSize = 8192;
	constexpr unsigned int SampleChannels = 2;
	using AudioFrame = std::array<int, FrameSize>;

	bool StartMusic();
	void Teardown();

	void _generator();
}
