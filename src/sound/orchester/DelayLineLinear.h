#pragma once

#include "Pipeline.h"

namespace blooDot
{
	namespace Orchestrator
	{
		class DelayLineLinear : public Pipeline
		{
		public:
			DelayLineLinear(double delay = 0., unsigned long maxDelay = (1 << 11) - 1);
			~DelayLineLinear() { };

			void SetDelay(double delay);
			void SetMaxDelay(unsigned long delay);
			double GetDelay(void) const;
			unsigned long GetMaxDelay(void);
			double GetOutputAt(unsigned long atDelay);
			void SetOutputAt(double value, unsigned long atDelay);
			double GetLastOutput(void) const;
			double GetNextOutput(void);
			double Render(double input);
			AudioFrames& Render(AudioFrames& frames, unsigned int channel = 0);
			AudioFrames& Render(AudioFrames& inFrames, AudioFrames &outFrames, unsigned int iChannel = 0, unsigned int oChannel = 0);

		private:
			unsigned long m_inputIndex;
			unsigned long m_outputIndex;
			double m_Delay;
			double m_scaleFactor;
			double m_fractionalPart;
			double m_nextOutput;
			bool m_enableNextOutput;
		};
	}
}