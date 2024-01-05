#pragma once

#include "flame-source.h"

namespace blooDot::Oscillator
{
	class RandomSource : public FlameSource
	{
	public:
		RandomSource(unsigned int randomizerSeed = 0);

		void InitializeRandomizer(unsigned int randomSeed = 0);

		double Render(void);
		AudioFrame& Render(AudioFrame& frames, unsigned int channel = 0);
		double GetLastOutput(void) const;
	};
}