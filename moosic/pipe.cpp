#include "pch.h"
#include "pipe.h"

namespace blooDot::Oscillator
{
	Pipe::Pipe(void)
	{
		this->m_channelsIn = 1;
		this->m_Gain = 1.;
		this->m_lastFrame.Resize(1, 1, 0.0);
	}

	unsigned int Pipe::GetChannelsIn(void) const
	{
		return this->m_channelsIn;
	}

	unsigned int Pipe::GetChannelsOut(void) const
	{
		return this->m_lastFrame.Channels();
	}

	void Pipe::SetGain(double gain)
	{
		this->m_Gain = gain;
	}

	double Pipe::GetGain(void) const
	{
		return this->m_Gain;
	}

	const AudioFrame& Pipe::GetLastFrame(void) const
	{
		return this->m_lastFrame;
	}

	inline void Pipe::Reset(void)
	{
		unsigned int i;
		for (i = 0; i < this->m_Inputs.Size(); ++i)
		{
			this->m_Inputs[i] = 0.0;
		}

		for (i = 0; i < this->m_Outputs.Size(); ++i)
		{
			this->m_Outputs[i] = 0.0;
		}

		for (i = 0; i < this->m_lastFrame.Size(); ++i)
		{
			this->m_lastFrame[i] = 0.0;
		}
	}

	inline double Pipe::GetPhaseDelay(double freq)
	{
		if (freq <= 0. || freq > .5 * OscillatorBase::SampleRate())
		{
			this->m_oStream << "Invalid phase delay frequency: " << freq;
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return 0.;
		}

		auto timeAngle = 2. * Oscillator::PI * freq / OscillatorBase::SampleRate();
		auto realPart = 0., imaginaryPart = 0.;
		for (unsigned int i = 0; i < this->m_pipelineB.size(); ++i)
		{
			realPart += this->m_pipelineB[i] * std::cos(i * timeAngle);
			imaginaryPart -= this->m_pipelineB[i] * std::sin(i * timeAngle);
		}

		realPart *= this->m_Gain;
		imaginaryPart *= this->m_Gain;
		auto phaseAngle = atan2(imaginaryPart, realPart);
		realPart = 0.;
		imaginaryPart = 0.;
		for (unsigned int i = 0; i < this->m_pipelineA.size(); ++i)
		{
			realPart += this->m_pipelineA[i] * std::cos(i * timeAngle);
			imaginaryPart -= this->m_pipelineA[i] * std::sin(i * timeAngle);
		}

		phaseAngle -= std::atan2(imaginaryPart, realPart);
		phaseAngle = std::fmod(-phaseAngle, 2. * Oscillator::PI);
		return phaseAngle / timeAngle;
	}
}