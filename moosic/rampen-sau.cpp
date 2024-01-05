#include "pch.h"
#include "rampen-sau.h"

namespace blooDot::Oscillator
{
	RampenSau::RampenSau(void) : FlameSource()
	{
		this->m_Status = false;
		this->m_Speed = .001;
		this->m_Value = 0.;
		this->m_Bound = 0.;
		OscillatorBase::AddSampleRateAlert(this);
	}

	RampenSau::~RampenSau(void)
	{
		OscillatorBase::RemoveSampleRateAlert(this);
	}

	RampenSau& RampenSau::operator= (const RampenSau& e)
	{
		if (&e != this)
		{
			this->m_Status = e.m_Status;
			this->m_Speed = e.m_Speed;
			this->m_Value = e.m_Value;
			this->m_Bound = e.m_Bound;
		}

		return *this;
	}

	void RampenSau::Enable(void)
	{
		this->SetBound(1.);
	}

	void RampenSau::Disable(void)
	{
		this->SetBound(0.);
	}

	void RampenSau::SetSampleRate(double newRate, double oldRate)
	{
		if (!this->m_ignoreSampleRateChange && newRate != 0.)
		{
			this->m_Speed = oldRate * this->m_Speed / newRate;
		}
	}

	void RampenSau::SetSpeed(double speed)
	{
		if (speed < 0.)
		{
			this->m_oStream << "Ramp-up speed must not be negative";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_Speed = speed;
	}

	void RampenSau::SetDuration(double duration)
	{
		if (duration <= 0.)
		{
			this->m_oStream << "Ramp-up duration must be greter than zero";
			this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			return;
		}

		this->m_Speed = 1. / (duration * OscillatorBase::SampleRate());
	}

	void RampenSau::SetBound(double bound)
	{
		this->m_Bound = bound;
		if (this->m_Value != this->m_Bound)
		{
			this->m_Status = true;
		}
	}

	void RampenSau::SetValue(double value)
	{
		this->m_Status = 0;
		this->m_Bound = value;
		this->m_Value = value;
		this->m_lastFrame[0] = this->m_Value;
	}

	inline double RampenSau::Render(void)
	{
		if (this->m_Status)
		{
			if (this->m_Bound > this->m_Value)
			{
				this->m_Value += this->m_Speed;
				if (this->m_Value >= this->m_Bound)
				{
					this->m_Value = this->m_Bound;
					this->m_Status = false;
				}
			}
			else
			{
				this->m_Value -= this->m_Speed;
				if (this->m_Value <= this->m_Bound)
				{
					this->m_Value = this->m_Bound;
					this->m_Status = false;
				}
			}

			this->m_lastFrame[0] = this->m_Value;
		}

		return this->m_Value;
	}

	inline AudioFrame& RampenSau::Render(AudioFrame& frames, unsigned int channel)
	{
		double* samples = &frames[channel];
		unsigned int stride = frames.Channels();

		for (unsigned int i = 0; i < frames.Frames(); ++i, samples += stride)
		{
			*samples = this->Render();
		}

		return frames;
	}

	bool RampenSau::GetStatus(void) const
	{
		return this->m_Status;
	}

	double RampenSau::GetLastFrame(void) const
	{
		return this->m_lastFrame[0];
	}
}