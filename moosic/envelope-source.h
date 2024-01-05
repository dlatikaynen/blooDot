#pragma once

#include "flame-source.h"

namespace blooDot::Oscillator
{
	class EnvelopeSource : public FlameSource
	{
	public:
		enum
		{
			ATTACK,
			SUSTAIN,
			RELEASE,
			DECAY,
			PEACE
		};

		EnvelopeSource(void);
		~EnvelopeSource(void);

		void Enable(void);
		void Disable(void);

		void SetAttack(double rate);
		void SetAttackDuration(double time);
		void SetAttackBound(double bound);
		void SetSustain(double level);
		void SetSustainActive(double value);
		void SetRelease(double rate);
		void SetReleaseDuration(double time);
		void SetDecay(double rate);
		void SetDecayDuration(double time);

		void SetPhasesDuration(double attTime, double susValue, double relTime, double decTime);
		void SetPhasesBound(double bound);

		int GetStatus(void) const;
		double GetLastFrame(void) const;
		double Render(void);
		AudioFrame& Render(AudioFrame& frames, unsigned int channel = 0);

	protected:
		void SetSampleRateInternal(double newRate, double oldRate);

		int m_Status;
		double m_Value;
		double m_Bound;
		double m_Attack;
		double m_sustainLevel;
		double m_Release;
		double m_releaseDuration;
		double m_Decay;
	};
}