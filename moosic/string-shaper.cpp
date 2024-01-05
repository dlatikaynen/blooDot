#include "pch.h"
#include "string-shaper.h"

namespace blooDot::Oscillator
{
	StringShape::StringShape(double minFreq)
	{
		if (minFreq <= 0.)
		{
			this->m_oStream << "String shape minimum frequency must be greater than zero";
			this->HandleException(OrchestratorExceptionType::FUNCTION_ARGUMENT);

			return;
		}

		auto delay = static_cast<unsigned long>(OscillatorBase::SampleRate() / minFreq);
		this->m_delayLineFilter.SetMaxDelay(delay + 1);
		this->m_delayLineLinear.SetMaxDelay(delay + 1);
		this->m_pickExcitation = .3;
		this->m_pickPosition = .4;
		this->m_mrTension = .9999;
		this->m_baseGain = .995;
		this->m_loopGain = .999;
		this->Reset();
		this->SetFreq(220.);
	}

	void StringShape::Reset(void)
	{
		this->m_delayLineFilter.Reset();
		this->m_delayLineLinear.Reset();
		this->m_noiseGate.Reset();
	}

	void StringShape::SetFreq(double freq)
	{
		this->m_mrFreq = freq;
		this->m_mrLen = OscillatorBase::SampleRate() / this->m_mrFreq;
		double delay = this->m_mrLen - .5;
		this->m_delayLineFilter.SetDelay(delay);
		this->m_loopGain = this->m_baseGain + (freq * .000005);
		if (this->m_loopGain >= 1.0)
		{
			this->m_loopGain = .99999;
		}

		this->SetTension(this->m_mrTension);
		this->m_delayLineLinear.SetDelay(.5 * this->m_pickPosition * this->m_mrLen);
	}

	void StringShape::SetTension(double tension)
	{
		this->m_mrTension = tension;
		double coefficient;
		double freq = this->m_mrFreq * 2.0;
		double dFreq = ((.5 * OscillatorBase::SampleRate()) - freq) * .25;
		double temp = .5 + (tension * .5);
		if (temp > .99999)
		{
			temp = .99999;
		}

		for (int i = 0; i < 4; ++i)
		{
			coefficient = temp * temp;
			this->m_secondOrder[i].SetA2(coefficient);
			this->m_secondOrder[i].SetB0(coefficient);
			this->m_secondOrder[i].SetB2(1.);
			coefficient = -2.0 * temp * ::cos(Oscillator::PI_2 * freq / OscillatorBase::SampleRate());
			this->m_secondOrder[i].SetA1(coefficient);
			this->m_secondOrder[i].SetB1(coefficient);
			freq += dFreq;
		}
	}

	void StringShape::SetPickPosition(double position) {

		if (position < 0. || position > 1.)
		{
			this->m_oStream << "Cannot pick a string outside its boundaries";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_pickPosition = position;
		this->m_delayLineLinear.SetDelay(.5 * this->m_pickPosition * this->m_mrLen);
	}

	void StringShape::SetBaseLoopGain(double gain)
	{
		m_baseGain = gain;
		this->m_loopGain = this->m_baseGain + (this->m_mrFreq * .000005);
		if (this->m_loopGain > .99999)
		{
			this->m_loopGain = .99999;
		}
	}

	void StringShape::Excite(double force)
	{
		if (force < 0. || force > 1.)
		{
			this->m_oStream << "String excitation force out of range";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_pickExcitation = force;
		for (unsigned long i = 0; i < m_mrLen; i++)
		{
			this->m_delayLineFilter.Render((this->m_delayLineFilter.GetLastOutput() * .6) + .4 * this->m_noiseSource.Render() * this->m_pickExcitation);
		}
	}

	void StringShape::Start(double freq, double volume)
	{
		this->SetFreq(freq);
		this->Excite(volume);
	}

	void StringShape::Stop(double volume)
	{
		if (volume < 0. || volume > 1.) {
			this->m_oStream << "String stopping force is out of range";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_loopGain = (1. - volume) * .5;
	}

	void StringShape::SetControl(int number, double value)
	{
		auto normalizedValue = value * Oscillator::INVERSE_HALFBYTE;
		if (number == 1) // modulate (pull)
		{
			this->SetTension(.9 + (.1 * (1. - normalizedValue)));
		}
		else if (number == 11) // expression
		{
			this->SetBaseLoopGain(.97 + (normalizedValue * .03));
		}
		if (number == 4) // pedal = postion of the pick/plucking
		{
			this->SetPickPosition(normalizedValue);
		}
	}

	double StringShape::Render(unsigned int)
	{
		auto preProcessed = this->m_delayLineFilter.GetLastOutput() * this->m_loopGain;
		preProcessed = this->m_secondOrder[0].Render(preProcessed);
		preProcessed = this->m_secondOrder[1].Render(preProcessed);
		preProcessed = this->m_secondOrder[2].Render(preProcessed);
		preProcessed = this->m_secondOrder[3].Render(preProcessed);
		preProcessed = this->m_noiseGate.Render(preProcessed);
		preProcessed = this->m_delayLineFilter.Render(preProcessed);
		return this->m_lastFrame[0] = preProcessed - this->m_delayLineLinear.Render(preProcessed);
	}

	AudioFrame& StringShape::Render(AudioFrame& frames, unsigned int channel)
	{
		auto nChannels = this->m_lastFrame.Channels();
		double* samples = &frames[channel];
		auto stride = frames.Channels() - nChannels;
		if (nChannels == 1)
		{
			for (unsigned int i = 0; i < frames.Frames(); i++, samples += stride)
			{
				*samples++ = this->Render();
			}
		}
		else
		{
			for (unsigned int i = 0; i < frames.Frames(); i++, samples += stride)
			{
				*samples++ = this->Render();
				for (unsigned int j = 1; j < nChannels; j++)
				{
					*samples++ = this->m_lastFrame[j];
				}
			}
		}

		return frames;
	}
}