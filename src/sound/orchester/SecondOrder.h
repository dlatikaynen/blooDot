#pragma once

#include "Pipeline.h"

namespace blooDot
{
	namespace Orchestrator
	{
		class SecondOrder : public Pipeline
		{
		public:
			SecondOrder();
			~SecondOrder();

			void SetB0(double value);
			void SetB1(double value);
			void SetB2(double value);
			void SetA1(double value);
			void SetA2(double value);
			void SetAllPipelineProperties(double b0, double b1, double b2, double a1, double a2, bool clearState = false);
			void SetResonance(double frequency, double radius, bool normalize = false);
			void SetAttenuation(double frequency, double radius);
			void SetEquipotential(void);

			void FreezeSampleRate(bool freeze = true);
			double Render(double input);
			AudioFrames& Render(AudioFrames& frames, unsigned int channel = 0);
			AudioFrames& Render(AudioFrames& inputFrames, AudioFrames &outputFrames, unsigned int inputChannel = 0, unsigned int outputChannel = 0);
			double GetLastOutput(void) const;

		protected:
			virtual void SetNewSampleRate(double newRate, double oldRate);
		};
	}
}
