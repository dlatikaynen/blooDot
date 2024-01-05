#pragma once

#include "pipe.h"

namespace blooDot::Oscillator
{
	class NoseGate : public Pipe
	{
	public:
		NoseGate(double centerPoint = -1.0);
		~NoseGate() { };

		void Calibrate(double centerPoint);
		void SetOffTreshold(double treshold);
		void SetOnTreshold(double treshold);
		void SetAllPipelineProperties(double offTreshold, double onTreshold, bool reset = false);

		double GetLastOutput(void) const;
		double Render(double input);
		AudioFrame& Render(AudioFrame& frames, unsigned int channel = 0);
		AudioFrame& Render(AudioFrame& inputFrames, AudioFrame& outputFrames, unsigned int inputChannel = 0, unsigned int outputChannel = 0);
	};
}