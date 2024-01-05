#pragma once

#include "synth.h"
#include "flame-source.h"
#include "random-noise.h"
#include "nose-gate.h"
#include "delay-line.h"
#include "delay-filter.h"
#include "derivative.h"

namespace blooDot::Oscillator
{
	class StringShape : public Synthesizer
	{
	public:
		StringShape(double minFreq = 100.);
		~StringShape(void) { };

		void SetFreq(double freq);
		void SetTension(double stretch);
		void SetPickPosition(double position);
		void SetBaseLoopGain(double gain);
		void Excite(double volume);
		void Start(double freq, double volume);
		void Stop(double volume);
		void Reset(void);
		void SetControl(int midiNumber, double value);

		double Render(unsigned int channel = 0);
		AudioFrame& Render(AudioFrame& frames, unsigned int channel = 0);

	private:
		double				m_baseGain;
		double				m_loopGain;
		double				m_mrLen;
		double				m_mrTension;
		double				m_pickPosition;
		double				m_pickExcitation;
		double				m_mrFreq;

		RandomSource		m_noiseSource;
		NoseGate			m_noiseGate;
		DelayLineFilter		m_delayLineFilter;
		DelayLine		m_delayLineLinear;
		Derivative			m_secondOrder[4];
	};
}