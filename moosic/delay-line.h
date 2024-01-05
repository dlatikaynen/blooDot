#pragma once

#include "pipe.h"

namespace blooDot::Oscillator
{
	class DelayLine : public Pipe
	{
	public:
		DelayLine(double delay = 0., unsigned long maxDelay = (1 << 11) - 1);
		~DelayLine() { };

		void SetDelay(double delay);
		void SetMaxDelay(unsigned long delay);
		double GetDelay(void) const;
		unsigned long GetMaxDelay(void);
		double GetOutputAt(unsigned long atDelay);
		void SetOutputAt(double value, unsigned long atDelay);
		double GetLastOutput(void) const;
		double GetNextOutput(void);
		double Render(double input);
		AudioFrame& Render(AudioFrame& frames, unsigned int channel = 0);
		AudioFrame& Render(AudioFrame& inFrames, AudioFrame& outFrames, unsigned int iChannel = 0, unsigned int oChannel = 0);

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