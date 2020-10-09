#include "DelayLineFilter.h"

namespace blooDot
{
	namespace Orchestrator
	{
		DelayLineFilter::DelayLineFilter(double delay, unsigned long maxDelay)
		{
			this->m_inputIndex = 0;
			this->m_filterInput = 0.;
			if (delay < .5)
			{
				this->m_oStream << "Delay must not be less than half an audio frame";
				this->HandleException(OrchestratorExceptionType::FUNCTION_ARGUMENT);
				return;
			}
			else if (delay > static_cast<double>(maxDelay))
			{
				this->m_oStream << "Delay value may not exceed maximum";
				this->HandleException(OrchestratorExceptionType::FUNCTION_ARGUMENT);
				return;
			}

			if (maxDelay + 1 > this->m_Inputs.Size())
			{
				this->m_Inputs.Resize(maxDelay + 1, 1, 0.0);
			}

			this->SetDelay(delay);
			this->m_enableNextOutput = true;
		}

		void DelayLineFilter::SetMaxDelay(unsigned long delay)
		{
			if (delay >= this->m_Inputs.Size())
			{
				this->m_Inputs.Resize(delay + 1, 1, 0.0);
			}
		}

		void DelayLineFilter::SetDelay(double delay)
		{
			unsigned long length = this->m_Inputs.Size();
			if ((delay + 1) > length)
			{
				this->m_oStream << "Delay must not exceed input buffer";
				this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
				return;
			}
			else if (delay < .5)
			{
				this->m_oStream << "Delay must not be smaller than one half frame";
				this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
			}

			auto outPointer = this->m_inputIndex - delay + 1.;
			this->m_Delay = delay;
			while (outPointer < 0)
			{
				outPointer += length;
			}

			this->m_outputIndex = (long)outPointer;
			if (this->m_outputIndex == length)
			{
				this->m_outputIndex = 0;
			}

			this->m_Factor = 1. + this->m_outputIndex - outPointer;
			if (m_Factor < .5)
			{
				++this->m_outputIndex;
				if (this->m_outputIndex >= length)
				{
					this->m_outputIndex -= length;
				}

				this->m_Factor += 1.;
			}

			this->m_Attenuation = (1. - this->m_Factor) / (1. + this->m_Factor);
		}

		double DelayLineFilter::GetDelayAt(unsigned long atTime)
		{
			auto index = this->m_inputIndex - atTime - 1;
			while (index < 0)
			{
				index += this->m_Inputs.Size();
			}

			return this->m_Inputs[index];
		}

		void DelayLineFilter::SetDelayAt(double value, unsigned long atTime)
		{
			auto index = this->m_inputIndex - atTime - 1;
			while (index < 0)
			{
				index += this->m_Inputs.Size();
			}

			this->m_Inputs[index] = value;
		}

		double DelayLineFilter::GetDelay(void) const
		{
			return this->m_Delay;
		}

		unsigned long DelayLineFilter::GetMaxDelay(void)
		{
			return this->m_Inputs.Size() - 1;
		}

		double DelayLineFilter::GetLastOutput(void) const
		{
			return this->m_lastFrame[0];
		}

		inline double DelayLineFilter::GetNextOutput(void)
		{
			if (this->m_enableNextOutput)
			{
				this->m_nextOutput = -this->m_Attenuation * this->m_lastFrame[0];
				this->m_nextOutput += this->m_filterInput + (this->m_Attenuation * this->m_Inputs[m_outputIndex]);
				this->m_enableNextOutput = false;
			}

			return this->m_nextOutput;
		}

		void DelayLineFilter::Reset()
		{
			for (unsigned int i = 0; i < this->m_Inputs.Size(); ++i)
			{
				this->m_Inputs[i] = 0.;
			}

			this->m_lastFrame[0] = 0.;
			this->m_filterInput = 0.;
		}

		inline double DelayLineFilter::Render(double input)
		{
			this->m_Inputs[m_inputIndex++] = input * this->m_Gain;
			if (this->m_inputIndex == this->m_Inputs.Size())
			{
				this->m_inputIndex = 0;
			}

			this->m_lastFrame[0] = this->GetNextOutput();
			this->m_enableNextOutput = true;
			this->m_filterInput = this->m_Inputs[m_outputIndex++];
			if (this->m_outputIndex == this->m_Inputs.Size())
			{
				this->m_outputIndex = 0;
			}

			return this->m_lastFrame[0];
		}

		inline AudioFrames& DelayLineFilter::Render(AudioFrames& frames, unsigned int channel)
		{
			double *samples = &frames[channel];
			unsigned int stride = frames.Channels();
			for (unsigned int i = 0; i < frames.Frames(); ++i, samples += stride)
			{
				this->m_Inputs[m_inputIndex++] = *samples * this->m_Gain;
				if (this->m_inputIndex == this->m_Inputs.Size())
				{
					this->m_inputIndex = 0;
				}

				*samples = this->GetNextOutput();
				this->m_lastFrame[0] = *samples;
				this->m_enableNextOutput = true;
				this->m_filterInput = this->m_Inputs[m_outputIndex++];
				if (this->m_outputIndex == this->m_Inputs.Size())
				{
					this->m_outputIndex = 0;
				}
			}

			return frames;
		}

		inline AudioFrames& DelayLineFilter::Render(
			AudioFrames& inputFrames,
			AudioFrames& outputFrames,
			unsigned int inputChannel,
			unsigned int outputChannel
		)
		{
			auto *inSamples = &inputFrames[inputChannel];
			auto inStride = inputFrames.Channels();
			auto *outSamples = &outputFrames[outputChannel];
			auto outStride = outputFrames.Channels();
			for (unsigned int i = 0; i < inputFrames.Frames(); ++i, inSamples += inStride, outSamples += outStride)
			{
				this->m_Inputs[m_inputIndex++] = *inSamples * this->m_Gain;
				if (this->m_inputIndex == this->m_Inputs.Size())
				{
					this->m_inputIndex = 0;
				}

				*outSamples = this->GetNextOutput();
				this->m_lastFrame[0] = *outSamples;
				this->m_enableNextOutput = true;
				this->m_filterInput = this->m_Inputs[m_outputIndex++];
				if (this->m_outputIndex == this->m_Inputs.Size())
				{
					this->m_outputIndex = 0;
				}
			}

			return inputFrames;
		}
	}
}