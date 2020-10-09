#pragma once

#include "OrchestratorException.h"
#include "OrchestratorBase.h"
#include "AudioFrames.h"
#include <vector>
#include <algorithm>
#include <cmath>

namespace blooDot
{
	namespace Orchestrator
	{
		class Pipeline : public OrchestratorBase
		{
		public:
			Pipeline(void);

			unsigned int GetChannelsIn(void) const;
			unsigned int GetChannelsOut(void) const;
			virtual void Clear(void);
			void SetGain(double gain);
			double GetGain(void) const;
			double GetPhaseDelay(double frequency);
			const AudioFrames& GetLastFrame(void) const;
			virtual AudioFrames& Render(AudioFrames& frames, unsigned int channel = 0) = 0;

		protected:
			unsigned int m_channelsIn;
			double m_Gain;
			AudioFrames m_lastFrame;
			AudioFrames m_Inputs;
			AudioFrames m_Outputs;
			std::vector<double> m_pipelineA;
			std::vector<double> m_pipelineB;
		};
	}
}