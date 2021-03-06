#pragma once

#include "Pipeline.h"

namespace blooDot
{
	namespace Orchestrator
	{
		class DelayLineFilter : public Pipeline
		{
		public:
			DelayLineFilter(double delay = .5, unsigned long maxDelay = (1 << 12) - 1);
			~DelayLineFilter() { };

			void SetDelay(double delay);
			void SetMaxDelay(unsigned long delay);
			double GetDelay(void) const;
			unsigned long GetMaxDelay(void);
			double GetDelayAt(unsigned long tapDelay);
			void SetDelayAt(double value, unsigned long tapDelay);
			double GetLastOutput(void) const;
			double GetNextOutput(void);
			void Reset(void);

			double Render(double input);
			AudioFrames& Render(AudioFrames& frames, unsigned int channel = 0);
			AudioFrames& Render(AudioFrames& inputFrames, AudioFrames& outputFrames, unsigned int inputChannel, unsigned int outputChannel);

		private:
			double			m_Delay;
			double			m_Factor;
			double			m_Attenuation;
			double			m_filterInput;

			unsigned long	m_inputIndex;
			unsigned long	m_outputIndex;
			double			m_nextOutput;
			bool			m_enableNextOutput;
		};
	}
}
