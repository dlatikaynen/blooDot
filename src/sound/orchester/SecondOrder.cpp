#include "SecondOrder.h"

namespace blooDot
{
	namespace Orchestrator
	{
		SecondOrder::SecondOrder()
			: Pipeline()
		{
			this->m_pipelineB.resize(3, 0.);
			this->m_pipelineA.resize(3, 0.);
			this->m_pipelineB[0] = 1.;
			this->m_pipelineA[0] = 1.;
			this->m_Inputs.Resize(3, 1, 0.);
			this->m_Outputs.Resize(3, 1, 0.);
			OrchestratorBase::AddSampleRateAlert(this);
		}

		SecondOrder::~SecondOrder()
		{
			OrchestratorBase::RemoveSampleRateAlert(this);
		}

		void SecondOrder::SetAllPipelineProperties(double b0, double b1, double b2, double a1, double a2, bool reset)
		{
			this->m_pipelineB[0] = b0;
			this->m_pipelineB[1] = b1;
			this->m_pipelineB[2] = b2;
			this->m_pipelineA[1] = a1;
			this->m_pipelineA[2] = a2;
			if (reset)
			{
				this->Reset();
			}
		}

		void SecondOrder::SetA1(double value)
		{
			this->m_pipelineA[1] = value;
		}

		void SecondOrder::SetA2(double value)
		{
			this->m_pipelineA[2] = value;
		}

		void SecondOrder::SetB0(double value)
		{
			this->m_pipelineB[0] = value;
		}

		void SecondOrder::SetB1(double value)
		{
			this->m_pipelineB[1] = value;
		}

		void SecondOrder::SetB2(double value)
		{
			this->m_pipelineB[2] = value;
		}

		void SecondOrder::SetResonance(double freq, double radius, bool gainCap)
		{
			this->m_pipelineA[2] = radius * radius;
			this->m_pipelineA[1] = -2. * radius * cos(Orchestrator::PI_2 * freq / OrchestratorBase::SampleRate());
			if (gainCap)
			{
				this->m_pipelineB[0] = .5 - .5 * this->m_pipelineA[2];
				this->m_pipelineB[1] = 0.;
				this->m_pipelineB[2] = -this->m_pipelineB[0];
			}
		}

		void SecondOrder::FreezeSampleRate(bool freeze)
		{
			this->m_ignoreSampleRateChange = freeze;
		}

		void SecondOrder::SetAttenuation(double freq, double radius)
		{
			this->m_pipelineB[2] = radius * radius;
			this->m_pipelineB[1] = -2. * radius * cos(Orchestrator::PI_2 * freq / OrchestratorBase::SampleRate());
		}

		void SecondOrder::SetEquipotential(void)
		{
			this->m_pipelineB[0] = 1.;
			this->m_pipelineB[1] = 0.;
			this->m_pipelineB[2] = -1.;
		}

		void SecondOrder::SetNewSampleRate(double newRate, double oldRate)
		{
			if (!this->m_ignoreSampleRateChange)
			{
				this->m_oStream << "Second order filter coefficients skewed due to frame-rate change";
				this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			}
		}

		inline double SecondOrder::Render(double input)
		{
			this->m_Inputs[0] = this->m_Gain * input;
			this->m_lastFrame[0] = this->m_pipelineB[0] * this->m_Inputs[0] + this->m_pipelineB[1] * this->m_Inputs[1] + this->m_pipelineB[2] * this->m_Inputs[2];
			this->m_lastFrame[0] -= this->m_pipelineA[2] * this->m_Outputs[2] + this->m_pipelineA[1] * this->m_Outputs[1];
			this->m_Inputs[2] = this->m_Inputs[1];
			this->m_Inputs[1] = this->m_Inputs[0];
			this->m_Outputs[2] = this->m_Outputs[1];
			this->m_Outputs[1] = this->m_lastFrame[0];
			return this->m_lastFrame[0];
		}

		inline AudioFrames& SecondOrder::Render(AudioFrames& frames, unsigned int channel)
		{
			double *samples = &frames[channel];
			unsigned int hop = frames.Channels();
			for (unsigned int i = 0; i < frames.Frames(); i++, samples += hop) {
				this->m_Inputs[0] = this->m_Gain * *samples;
				*samples = this->m_pipelineB[0] * this->m_Inputs[0] + this->m_pipelineB[1] * this->m_Inputs[1] + this->m_pipelineB[2] * this->m_Inputs[2];
				*samples -= this->m_pipelineA[2] * this->m_Outputs[2] + this->m_pipelineA[1] * this->m_Outputs[1];
				this->m_Inputs[2] = this->m_Inputs[1];
				this->m_Inputs[1] = this->m_Inputs[0];
				this->m_Outputs[2] = this->m_Outputs[1];
				this->m_Outputs[1] = *samples;
			}

			this->m_lastFrame[0] = this->m_Outputs[1];
			return frames;
		}

		inline AudioFrames& SecondOrder::Render(
			AudioFrames& inputFrames,
			AudioFrames& outputFrames,
			unsigned int inputChannel,
			unsigned int outputChannel
		)
		{
			auto *iSamples = &inputFrames[inputChannel];
			auto inStride = inputFrames.Channels();
			auto *oSamples = &outputFrames[outputChannel];
			auto outStride = outputFrames.Channels();
			for (unsigned int i = 0; i < inputFrames.Frames(); ++i, iSamples += inStride, oSamples += outStride)
			{
				this->m_Inputs[0] = this->m_Gain * (*iSamples);
				*oSamples = this->m_pipelineB[0] * this->m_Inputs[0] + this->m_pipelineB[1] * this->m_Inputs[1] + this->m_pipelineB[2] * this->m_Inputs[2];
				*oSamples -= this->m_pipelineA[2] * this->m_Outputs[2] + this->m_pipelineA[1] * this->m_Outputs[1];
				this->m_Inputs[2] = this->m_Inputs[1];
				this->m_Inputs[1] = this->m_Inputs[0];
				this->m_Outputs[2] = this->m_Outputs[1];
				this->m_Outputs[1] = *oSamples;
			}

			this->m_lastFrame[0] = this->m_Outputs[1];
			return inputFrames;
		}

		double SecondOrder::GetLastOutput(void) const
		{
			return this->m_lastFrame[0];
		}
	}
}
