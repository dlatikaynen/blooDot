#include "RandomSource.h"
#include <time.h>

namespace blooDot
{
	namespace Orchestrator
	{
		RandomSource::RandomSource(unsigned int randomizerSeed)
		{
			this->InitializeRandomizer(randomizerSeed);
		}

		void RandomSource::InitializeRandomizer(unsigned int randomSeed)
		{
			if (randomSeed == 0)
			{
				::srand(static_cast<unsigned int>(::time(NULL)));
			}
			else
			{
				::srand(randomSeed);
			}
		}

		double RandomSource::Render(void)
		{
			return this->m_lastFrame[0] = static_cast<double>(2. * ::rand() / (RAND_MAX + 1.) - 1.);
		}

		AudioFrames& RandomSource::Render(AudioFrames& frames, unsigned int channel)
		{
			auto *samples = &frames[channel];
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
}
