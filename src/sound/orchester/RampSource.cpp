#include "RampSource.h"

namespace blooDot
{
	namespace Orchestrator
	{
		RampSource::RampSource(void) : FrameSource()
		{
			this->m_Status = false;
			this->m_Speed = .001;
			this->m_Value = 0.;
			this->m_Bound = 0.;
			OrchestratorBase::AddSampleRateAlert(this);
		}

		RampSource::~RampSource(void)
		{
			OrchestratorBase::RemoveSampleRateAlert(this);
		}

		RampSource& RampSource::operator= (const RampSource& e)
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

		void RampSource::Enable(void)
		{
			this->SetBound(1.);
		}

		void RampSource::Disable(void)
		{
			this->SetBound(0.);
		}

		void RampSource::SetSampleRate(double newRate, double oldRate)
		{
			if (!this->m_ignoreSampleRateChange && newRate != 0.)
			{
				this->m_Speed = oldRate * this->m_Speed / newRate;
			}
		}

		void RampSource::SetSpeed(double speed)
		{
			if (speed < 0.)
			{
				this->m_oStream << "Ramp-up speed must not be negative";
				this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
				return;
			}

			this->m_Speed = speed;
		}

		void RampSource::SetDuration(double duration)
		{
			if (duration <= 0.)
			{
				this->m_oStream << "Ramp-up duration must be greter than zero";
				this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
				return;
			}

			this->m_Speed = 1. / (duration * OrchestratorBase::SampleRate());
		}

		void RampSource::SetBound(double bound)
		{
			this->m_Bound = bound;
			if (this->m_Value != this->m_Bound)
			{
				this->m_Status = true;
			}
		}

		void RampSource::SetValue(double value)
		{
			this->m_Status = 0;
			this->m_Bound = value;
			this->m_Value = value;
			this->m_lastFrame[0] = this->m_Value;
		}

		inline double RampSource::Render(void)
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

		inline AudioFrames& RampSource::Render(AudioFrames& frames, unsigned int channel)
		{
			double *samples = &frames[channel];
			unsigned int stride = frames.Channels();
			for (unsigned int i = 0; i < frames.Frames(); ++i, samples += stride)
			{
				*samples = this->Render();
			}

			return frames;
		}

		bool RampSource::GetStatus(void) const
		{
			return this->m_Status;
		}

		double RampSource::GetLastFrame(void) const
		{
			return this->m_lastFrame[0];
		}
	}
}
