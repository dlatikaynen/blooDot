#pragma once

#include "flame-source.h"

namespace blooDot::Oscillator
{
	class RampenSau : public FlameSource
	{
	public:
		RampenSau(void);
		~RampenSau(void);

		RampenSau& operator= (const RampenSau& e);

		void Enable(void);
		void Disable(void);
		void SetSpeed(double speed);
		void SetDuration(double duration);
		void SetBound(double bound);
		void SetValue(double value);
		bool GetStatus(void) const;
		double GetLastFrame(void) const;
		double Render(void);
		AudioFrame& Render(AudioFrame& frames, unsigned int channel = 0);

	protected:
		void SetSampleRate(double newRate, double oldRate);

		bool	m_Status;
		double	m_Value;
		double	m_Speed;
		double	m_Bound;
	};
}