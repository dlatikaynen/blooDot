#pragma once

#include "Pipeline.h"

namespace blooDot
{
	namespace Orchestrator
	{
		class NoiseGate : public Pipeline
		{
		public:
			NoiseGate(double centerPoint = -1.0);
			~NoiseGate() { };

			void Calibrate(double centerPoint);
			void SetOffTreshold(double treshold);
			void SetOnTreshold(double treshold);
			void SetAllPipelineProperties(double offTreshold, double onTreshold, bool reset = false);

			double GetLastOutput(void) const;
			double Render(double input);
			AudioFrames& Render(AudioFrames& frames, unsigned int channel = 0);
			AudioFrames& Render(AudioFrames& inputFrames, AudioFrames &outputFrames, unsigned int inputChannel = 0, unsigned int outputChannel = 0);
		};
	}
}