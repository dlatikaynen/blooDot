#pragma once
#include "oscillator-exception.h"
#include "oscillator-base.h"
#include "frame.h"

namespace blooDot::Oscillator
{
	class Synthesizer : public OscillatorBase
	{
	public:
		Synthesizer(void) { this->m_lastFrame.Resize(1, 1, 0.0); };

		virtual void Clear(void) { };
		virtual void Start(double frequency, double amplitude) = 0;
		virtual void Stop(double amplitude) = 0;
		virtual void SetFreq(double frequency);
		virtual void SetCtrl(int number, double value);
		unsigned int GetChannelsOut(void) const { return this->m_lastFrame.Channels(); };
		const AudioFrame& LastFrame(void) const { return this->m_lastFrame; };
		double LastOut(unsigned int channel = 0);
		virtual double Render(unsigned int channel = 0) = 0;
		virtual AudioFrame& Render(AudioFrame& frames, unsigned int channel = 0) = 0;

	protected:
		AudioFrame m_lastFrame;
	};

	inline void Synthesizer::SetFreq(double)
	{
		/* pure virtual function call */
		this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
	}

	inline double Synthesizer::LastOut(unsigned int channel)
	{
		return this->m_lastFrame[channel];
	}

	inline void Synthesizer::SetCtrl(int, double)
	{
		this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
	}
}