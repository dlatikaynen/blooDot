#include "pch.h"
#include "nose-gate.h"

namespace blooDot::Oscillator
{
	NoseGate::NoseGate(double centerPoint)
	{
		this->m_pipelineB.resize(2);
		this->m_Inputs.Resize(2, 1, 0.0);
		this->Calibrate(centerPoint);
	}

	void NoseGate::Calibrate(double centerPoint)
	{
		if (centerPoint > 0.)
		{
			this->m_pipelineB[0] = 1. / (1. + centerPoint);
		}
		else
		{
			this->m_pipelineB[0] = 1. / (1. - centerPoint);
		}

		this->m_pipelineB[1] = -centerPoint * this->m_pipelineB[0];
	}

	void NoseGate::SetOffTreshold(double treshold)
	{
		this->m_pipelineB[0] = treshold;
	}

	void NoseGate::SetOnTreshold(double treshold)
	{
		this->m_pipelineB[1] = treshold;
	}

	void NoseGate::SetAllPipelineProperties(double offTreshold, double onTreshold, bool reset)
	{
		this->m_pipelineB[0] = offTreshold;
		this->m_pipelineB[1] = onTreshold;
		if (reset)
		{
			this->Reset();
		}
	}

	double NoseGate::GetLastOutput(void) const
	{
		return this->m_lastFrame[0];
	}

	double NoseGate::Render(double input)
	{
		this->m_Inputs[0] = this->m_Gain * input;
		this->m_lastFrame[0] = this->m_pipelineB[1] * this->m_Inputs[1] + this->m_pipelineB[0] * this->m_Inputs[0];
		this->m_Inputs[1] = this->m_Inputs[0];

		return this->m_lastFrame[0];
	}

	AudioFrame& NoseGate::Render(AudioFrame& frames, unsigned int channel)
	{
		auto* samples = &frames[channel];
		auto stride = frames.Channels();

		for (unsigned int i = 0; i < frames.Frames(); ++i, samples += stride)
		{
			this->m_Inputs[0] = this->m_Gain * (*samples);
			*samples = this->m_pipelineB[1] * this->m_Inputs[1] + this->m_pipelineB[0] * this->m_Inputs[0];
			this->m_Inputs[1] = this->m_Inputs[0];
		}

		this->m_lastFrame[0] = *(samples - stride);

		return frames;
	}

	AudioFrame& NoseGate::Render(
		AudioFrame& inputFrames,
		AudioFrame& outputFrames,
		unsigned int inputChannel,
		unsigned int outputChannel
	)
	{
		auto* iSamples = &inputFrames[inputChannel];
		auto inStride = inputFrames.Channels();
		auto* oSamples = &outputFrames[outputChannel];
		auto outStride = outputFrames.Channels();

		for (unsigned int i = 0; i < inputFrames.Frames(); ++i, iSamples += inStride, oSamples += outStride)
		{
			this->m_Inputs[0] = this->m_Gain * (*iSamples);
			*oSamples = this->m_pipelineB[1] * this->m_Inputs[1] + this->m_pipelineB[0] * this->m_Inputs[0];
			this->m_Inputs[1] = this->m_Inputs[0];
		}

		this->m_lastFrame[0] = *(oSamples - outStride);

		return inputFrames;
	}
}