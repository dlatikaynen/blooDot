#include "DelayLineLinear.h"

namespace blooDot
{
	namespace Orchestrator
	{
		DelayLineLinear::DelayLineLinear(double delay, unsigned long maxDelay)
		{
			this->m_inputIndex = 0;
			if (delay < 0.)
			{
				this->m_oStream << "Initial line delay must not be negative";
				this->HandleException(OrchestratorExceptionType::FUNCTION_ARGUMENT);
				return;
			}
			else if (delay > static_cast<double>(maxDelay))
			{
				this->m_oStream << "Maximum line delay must not be exceeded";
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

		void DelayLineLinear::SetMaxDelay(unsigned long delay)
		{
			if (delay < this->m_Inputs.Size())
			{
				return;
			}

			this->m_Inputs.Resize(delay + 1, 1, 0.0);
		}

		double DelayLineLinear::GetOutputAt(unsigned long atTime)
		{
			auto index = this->m_inputIndex - atTime - 1;
			while (index < 0)
			{
				index += this->m_Inputs.Size();
			}

			return this->m_Inputs[index];
		}

		void DelayLineLinear::SetOutputAt(double value, unsigned long atTime)
		{
			auto index = this->m_inputIndex - atTime - 1;
			while (index < 0)
			{
				index += this->m_Inputs.Size();
			}

			this->m_Inputs[index] = value;
		}

		inline double DelayLineLinear::GetNextOutput(void)
		{
			if (this->m_enableNextOutput)
			{
				this->m_nextOutput = this->m_Inputs[this->m_outputIndex] * this->m_fractionalPart;
				if (this->m_outputIndex + 1 < this->m_Inputs.Size())
				{
					this->m_nextOutput += this->m_Inputs[this->m_outputIndex + 1] * this->m_scaleFactor;
				}
				else
				{
					this->m_nextOutput += this->m_Inputs[0] * this->m_scaleFactor;
				}

				this->m_enableNextOutput = false;
			}

			return this->m_nextOutput;
		}

		inline void DelayLineLinear::SetDelay(double delay)
		{
			if (delay < 0)
			{
				this->m_oStream << "Delay must not be negative";
				this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
				return;
			}
			else if ((delay + 1) > this->m_Inputs.Size())
			{
				this->m_oStream << "Cannot delay beyond end of audio input buffer";
				this->HandleException(OrchestratorExceptionType::WARNING_MESSAGE);
				return;
			}

			double outputIndex = this->m_inputIndex - delay; 
			this->m_Delay = delay;
			while (outputIndex < 0)
			{
				outputIndex += this->m_Inputs.Size();
			}

			this->m_outputIndex = (long)outputIndex;
			this->m_scaleFactor = outputIndex - this->m_outputIndex;
			this->m_fractionalPart = (double) 1.0 - this->m_scaleFactor;
			if (this->m_outputIndex == this->m_Inputs.Size())
			{
				this->m_outputIndex = 0;
			}

			this->m_enableNextOutput = true;
		}

		inline double DelayLineLinear::Render(double input)
		{
			this->m_Inputs[this->m_inputIndex++] = input * this->m_Gain;
			if (this->m_inputIndex == this->m_Inputs.Size())
			{
				this->m_inputIndex = 0;
			}

			this->m_lastFrame[0] = this->GetNextOutput();
			this->m_enableNextOutput = true;
			if (++this->m_outputIndex == this->m_Inputs.Size())
			{
				this->m_outputIndex = 0;
			}

			return this->m_lastFrame[0];
		}

		inline AudioFrames& DelayLineLinear::Render(AudioFrames& frames, unsigned int channel)
		{
			double *samples = &frames[channel];
			unsigned int stride = frames.Channels();
			for (unsigned int i = 0; i < frames.Frames(); ++i, samples += stride)
			{
				this->m_Inputs[this->m_inputIndex++] = *samples * this->m_Gain;
				if (this->m_inputIndex == this->m_Inputs.Size()) this->m_inputIndex = 0;
				*samples = this->GetNextOutput();
				this->m_enableNextOutput = true;
				if (++this->m_outputIndex == this->m_Inputs.Size())
				{
					this->m_outputIndex = 0;
				}
			}

			this->m_lastFrame[0] = *(samples - stride);
			return frames;
		}

		inline AudioFrames& DelayLineLinear::Render(
			AudioFrames& inputFrames,
			AudioFrames& outputFrames,
			unsigned int iChannel,
			unsigned int oChannel
		)
		{
			double *inFrames = &inputFrames[iChannel];
			unsigned int inStride = inputFrames.Channels();
			double *outFrames = &outputFrames[oChannel];
			unsigned int outStride = outputFrames.Channels();
			for (unsigned int i = 0; i < inputFrames.Frames(); i++, inFrames += inStride, outFrames += outStride)
			{
				this->m_Inputs[this->m_inputIndex++] = *inFrames * this->m_Gain;
				if (this->m_inputIndex == this->m_Inputs.Size())
				{
					this->m_inputIndex = 0;
				}

				*outFrames = this->GetNextOutput();
				this->m_enableNextOutput = true;
				if (++this->m_outputIndex == this->m_Inputs.Size())
				{
					this->m_outputIndex = 0;
				}
			}

			this->m_lastFrame[0] = *(outFrames - outStride);
			return inputFrames;
		}

		double DelayLineLinear::GetDelay(void) const
		{
			return this->m_Delay;
		}

		unsigned long DelayLineLinear::GetMaxDelay(void)
		{
			return this->m_Inputs.Size() - 1;
		}

		double DelayLineLinear::GetLastOutput(void) const
		{
			return this->m_lastFrame[0];
		}
	}
}
