#pragma once

#include "FrameSource.h"

namespace blooDot
{
	namespace Orchestrator
	{
		class RampSource : public FrameSource
		{
		public:
			RampSource(void);
			~RampSource(void);

			RampSource& operator= (const RampSource& e);

			void Enable(void);
			void Disable(void);
			void SetSpeed(double speed);
			void SetDuration(double duration);
			void SetBound(double bound);
			void SetValue(double value);
			bool GetStatus(void) const;
			double GetLastFrame(void) const;
			double Render(void);
			AudioFrames& Render(AudioFrames& frames, unsigned int channel = 0);

		protected:
			void SetSampleRate(double newRate, double oldRate);

			bool	m_Status;
			double	m_Value;
			double	m_Speed;
			double	m_Bound;
		};
	}
}
