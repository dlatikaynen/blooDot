#include "pch.h"
#include "random-noise.h"
#include <time.h>
#include <random>

namespace blooDot::Music 
{
	extern std::mt19937 gen;
	extern std::uniform_real_distribution<> noise;
}

namespace blooDot::Oscillator
{
	using namespace blooDot::Music;

	double RandomSource::Render(void)
	{
		return this->m_lastFrame[0] = noise(gen);
	}

	AudioFrame& RandomSource::Render(AudioFrame& frames, unsigned int channel)
	{
		auto* samples = &frames[channel];
		auto stride = frames.Channels();

		for (unsigned int i = 0; i < frames.Frames(); ++i, samples += stride)
		{
			*samples = static_cast<double>(2. * ::rand() / (RAND_MAX + 1.) - 1.);
		}

		this->m_lastFrame[0] = *(samples - stride);

		return frames;
	}

	double RandomSource::GetLastOutput(void) const { return this->m_lastFrame[0]; };
}