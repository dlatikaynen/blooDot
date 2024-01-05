#pragma once

#include "oscillator-exception.h"
#include "oscillator-base.h"
#include "frame.h"
#include <vector>
#include <algorithm>
#include <cmath>

namespace blooDot::Oscillator
{
	class Pipe : public OscillatorBase
	{
	public:
		Pipe(void);

		unsigned int GetChannelsIn(void) const;
		unsigned int GetChannelsOut(void) const;
		virtual void Reset(void);
		void SetGain(double gain);
		double GetGain(void) const;
		double GetPhaseDelay(double frequency);
		const AudioFrame& GetLastFrame(void) const;
		virtual AudioFrame& Render(AudioFrame& frames, unsigned int channel = 0) = 0;

	protected:
		unsigned int m_channelsIn;
		double m_Gain;
		AudioFrame m_lastFrame;
		AudioFrame m_Inputs;
		AudioFrame m_Outputs;
		std::vector<double> m_pipelineA;
		std::vector<double> m_pipelineB;
	};
}