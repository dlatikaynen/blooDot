#pragma once

#include "flame-source.h"

namespace blooDot::Oscillator
{
	class RandomSource : public FlameSource
	{
	public:
		RandomSource() {}

		double Render(void);
		AudioFrame& Render(AudioFrame& frames, unsigned int channel = 0);
		double GetLastOutput(void) const;
	};
}