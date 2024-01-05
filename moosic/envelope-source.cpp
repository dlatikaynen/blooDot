#include "pch.h"
#include "envelope-source.h"

namespace blooDot::Oscillator
{
	EnvelopeSource::EnvelopeSource(void)
	{
		this->m_Value = 0.;
		this->m_Attack = .001;
		this->m_Decay = .001;
		this->m_Release = .005;
		this->m_releaseDuration = -1.;
		this->m_sustainLevel = .5;
		this->m_Bound = 0.;
		this->m_Status = EnvelopeSource::PEACE;
		OscillatorBase::AddSampleRateAlert(this);
	}

	EnvelopeSource::~EnvelopeSource(void)
	{
		OscillatorBase::RemoveSampleRateAlert(this);
	}

	void EnvelopeSource::Enable()
	{
		if (this->m_Bound <= 0.)
		{
			this->m_Bound = 1.;
		}

		this->m_Status = EnvelopeSource::ATTACK;
	}

	void EnvelopeSource::Disable()
	{
		this->m_Bound = 0.;
		this->m_Status = EnvelopeSource::RELEASE;
		if (this->m_releaseDuration > 0.)
		{
			this->m_Release = this->m_Value / (this->m_releaseDuration * OscillatorBase::SampleRate());
		}
	}

	void EnvelopeSource::SetAttack(double attack)
	{
		if (attack < 0.)
		{
			this->m_oStream << "Envelope attack speed must not be negative";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_Attack = attack;
	}

	void EnvelopeSource::SetAttackDuration(double attack)
	{
		if (attack <= 0.)
		{
			this->m_oStream << "Envelope attack duration must be greater than zero";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_Attack = 1. / (attack * OscillatorBase::SampleRate());
	}

	void EnvelopeSource::SetAttackBound(double bound)
	{
		if (bound < 0.)
		{
			this->m_oStream << "Envelope attack upper bound must not be negative";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_Bound = bound;
	}

	void EnvelopeSource::SetSustain(double sustain)
	{
		if (sustain < 0.)
		{
			this->m_oStream << "Envelope sustain value must not be negative";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_sustainLevel = sustain;
	}

	void EnvelopeSource::SetSustainActive(double value)
	{
		this->m_Status = EnvelopeSource::SUSTAIN;
		this->m_Bound = value;
		this->m_Value = value;
		this->SetSustain(value);
		this->m_lastFrame[0] = value;
	}

	void EnvelopeSource::SetRelease(double release)
	{
		if (release < 0.)
		{
			this->m_oStream << "Envelope release speed must not be negative";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_Release = release;
		this->m_releaseDuration = -1.;
	}

	void EnvelopeSource::SetReleaseDuration(double release)
	{
		if (release <= 0.)
		{
			this->m_oStream << "Envelope release duration must be greater than zero";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_Release = this->m_sustainLevel / (release * OscillatorBase::SampleRate());
		this->m_releaseDuration = release;
	}

	void EnvelopeSource::SetDecay(double decay)
	{
		if (decay < 0.)
		{
			this->m_oStream << "Envelope decay speed must not be negative";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_Decay = decay;
	}

	void EnvelopeSource::SetDecayDuration(double decay)
	{
		if (decay <= 0.)
		{
			this->m_oStream << "Envelope decay duration must be greater than zero";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_Decay = (1. - this->m_sustainLevel) / (decay * OscillatorBase::SampleRate());
	}

	void EnvelopeSource::SetPhasesDuration(double attTime, double susValue, double relTime, double decTime)
	{
		this->SetAttackDuration(attTime);
		this->SetSustain(susValue);
		this->SetReleaseDuration(relTime);
		this->SetDecayDuration(decTime);
	}

	void EnvelopeSource::SetPhasesBound(double bound)
	{
		if (bound < 0.)
		{
			this->m_oStream << "Envelope slope bound must not be negative";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_Bound = bound;
		this->SetSustain(this->m_Bound);
		if (this->m_Value < this->m_Bound)
		{
			this->m_Status = EnvelopeSource::ATTACK;
		}
		else if (this->m_Value > this->m_Bound)
		{
			this->m_Status = EnvelopeSource::DECAY;
		}
	}

	void EnvelopeSource::SetSampleRateInternal(double newRate, double oldRate)
	{
		if (!this->m_ignoreSampleRateChange && newRate != 0.)
		{
			this->m_Attack = oldRate * this->m_Attack / newRate;
			this->m_Release = oldRate * this->m_Release / newRate;
			this->m_Decay = oldRate * this->m_Decay / newRate;
		}
	}

	/* ATTACK / DECAY ^ SUSTAIN --- RELEASE \ */
	inline double EnvelopeSource::Render(void)
	{
		switch (this->m_Status)
		{
		case EnvelopeSource::ATTACK:
		{
			this->m_Value += this->m_Attack;
			if (this->m_Value >= this->m_Bound)
			{
				this->m_Value = this->m_Bound;
				this->m_Bound = this->m_sustainLevel;
				this->m_Status = EnvelopeSource::DECAY;
			}

			this->m_lastFrame[0] = this->m_Value;
		}

		break;

		case EnvelopeSource::RELEASE:
		{
			this->m_Value -= this->m_Release;
			if (this->m_Value <= 0.)
			{
				this->m_Value = 0.;
				this->m_Status = EnvelopeSource::PEACE;
			}

			this->m_lastFrame[0] = this->m_Value;
		}

		break;

		case EnvelopeSource::DECAY:
		{
			if (this->m_Value > this->m_sustainLevel)
			{
				this->m_Value -= this->m_Decay;
				if (this->m_Value <= this->m_sustainLevel)
				{
					this->m_Value = this->m_sustainLevel;
					this->m_Status = EnvelopeSource::SUSTAIN;
				}
			}
			else
			{
				this->m_Value += this->m_Decay;
				if (this->m_Value >= this->m_sustainLevel)
				{
					this->m_Value = this->m_sustainLevel;
					this->m_Status = EnvelopeSource::SUSTAIN;
				}
			}

			this->m_lastFrame[0] = this->m_Value;
		}

		break;

		}

		return this->m_Value;
	}

	inline AudioFrame& EnvelopeSource::Render(AudioFrame& frames, unsigned int channel)
	{
		double* samples = &frames[channel];
		unsigned int stride = frames.Channels();

		for (unsigned int i = 0; i < frames.Frames(); ++i, samples += stride)
		{
			*samples = this->Render();
		}

		return frames;
	}

	int EnvelopeSource::GetStatus(void) const
	{
		return this->m_Status;
	}

	double EnvelopeSource::GetLastFrame(void) const
	{
		return this->m_lastFrame[0];
	}
}