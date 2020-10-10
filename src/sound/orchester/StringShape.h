#pragma once

#include "Synthesizer.h"
#include "FrameSource.h"
#include "RandomSource.h"
#include "NoiseGate.h"
#include "DelayLineLinear.h"
#include "DelayLineFilter.h"
#include "SecondOrder.h"

namespace blooDot
{
	namespace Orchestrator
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
			AudioFrames& Render(AudioFrames& frames, unsigned int channel = 0);

		private:
			double				m_baseGain;
			double				m_loopGain;
			double				m_mrLen;
			double				m_mrTension;
			double				m_pickPosition;
			double				m_pickExcitation;
			double				m_mrFreq;

			RandomSource		m_noiseSource;
			NoiseGate			m_noiseGate;
			DelayLineFilter		m_delayLineFilter;
			DelayLineLinear		m_delayLineLinear;
			SecondOrder			m_secondOrder[4];
		};
	}
}
