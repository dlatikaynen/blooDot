#pragma once

#include "FrameSource.h"

namespace blooDot
{
	namespace Orchestrator
	{
		class RandomSource : public FrameSource
		{
		public:
			RandomSource(unsigned int randomizerSeed = 0);
			
			void InitializeRandomizer(unsigned int randomSeed = 0);

			double Render(void);
			AudioFrames& Render(AudioFrames& frames, unsigned int channel = 0);
			double GetLastOutput(void) const;
		};
	}
}