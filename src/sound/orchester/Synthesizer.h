#pragma once

#include "OrchestratorException.h"
#include "OrchestratorBase.h"
#include "AudioFrames.h"

namespace blooDot
{
	namespace Orchestrator
	{
		class Synthesizer : public OrchestratorBase
		{
		public:
			Synthesizer(void) { this->m_lastFrame.Resize(1, 1, 0.0); };
			
			virtual void Clear(void) { };
			virtual void Start(double frequency, double amplitude) = 0;
			virtual void Stop(double amplitude) = 0;
			virtual void SetFreq(double frequency);
			virtual void SetCtrl(int number, double value);
			unsigned int GetChannelsOut(void) const { return this->m_lastFrame.Channels(); };
			const AudioFrames& LastFrame(void) const { return this->m_lastFrame; };
			double LastOut(unsigned int channel = 0);
			virtual double Render(unsigned int channel = 0) = 0;
			virtual AudioFrames& Render(AudioFrames& frames, unsigned int channel = 0) = 0;

		protected:
			AudioFrames m_lastFrame;
		};

		inline void Synthesizer::SetFreq(double frequency)
		{
			/* pure virtual function call */
			HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
		}

		inline double Synthesizer::LastOut(unsigned int channel)
		{
			return this->m_lastFrame[channel];
		}

		inline void Synthesizer::SetCtrl(int number, double value)
		{			
			HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
		}
	}
}