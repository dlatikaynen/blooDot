#include "OrchestratorException.h"
#include "OrchestratorBase.h"
#include "AudioFrames.h"

namespace blooDot
{
	namespace Orchestrator
	{
		AudioFrames::AudioFrames(unsigned int nFrames, unsigned int nChannels) :
			m_Data(0),
			m_nFrames(nFrames),
			m_nChannels(nChannels)
		{
			this->m_Size = this->m_nFrames * this->m_nChannels;
			this->m_bufferSize = this->m_Size;
			if (this->m_Size > 0)
			{
				this->m_Data = (double*)calloc(this->m_Size, sizeof(double));
				if (this->m_Data == nullptr)
				{
					std::string error = "Out of memory in AudioFrame storage allocation";
					OrchestratorBase::HandleException(error, OrchestratorExceptionType::MEMORY_ALLOCATION);
				}
			}

			this->m_dataRate = OrchestratorBase::SampleRate();
		}

		AudioFrames::AudioFrames(const double& value, unsigned int nFrames, unsigned int nChannels) :
			m_Data(0),
			m_nFrames(nFrames),
			m_nChannels(nChannels)
		{
			this->m_Size = this->m_nFrames * this->m_nChannels;
			this->m_bufferSize = this->m_Size;
			if (this->m_Size > 0)
			{
				this->m_Data = (double *)malloc(this->m_Size * sizeof(double));
				if (this->m_Data == NULL)
				{
					std::string error = "Out of memory in audio frame storage allocation";
					OrchestratorBase::HandleException(error, OrchestratorExceptionType::MEMORY_ALLOCATION);
				}

				for (long i = 0; i < (long)this->m_Size; i++)
				{
					this->m_Data[i] = value;
				}
			}

			this->m_dataRate = OrchestratorBase::SampleRate();
		}

		AudioFrames::~AudioFrames()
		{
			if (this->m_Data)
			{
				free(this->m_Data);
			}
		}

		AudioFrames::AudioFrames(const AudioFrames& f) :
			m_Data(0),
			m_Size(0),
			m_bufferSize(0)
		{
			this->Resize(f.Frames(), f.Channels());
			this->m_dataRate = OrchestratorBase::SampleRate();
			for (unsigned int i = 0; i < this->m_Size; i++)
			{
				this->m_Data[i] = f[i];
			}
		}

		AudioFrames& AudioFrames :: operator= (const AudioFrames& f)
		{
			if (this->m_Data)
			{
				free(this->m_Data);
			}

			this->m_Data = 0;
			this->m_Size = 0;
			this->m_bufferSize = 0;
			Resize(f.Frames(), f.Channels());
			this->m_dataRate = OrchestratorBase::SampleRate();
			for (unsigned int i = 0; i < this->m_Size; i++)
			{
				this->m_Data[i] = f[i];
			}

			return *this;
		}

		void AudioFrames::Resize(size_t nFrames, unsigned int nChannels)
		{
			this->m_nFrames = nFrames;
			this->m_nChannels = nChannels;
			this->m_Size = this->m_nFrames * this->m_nChannels;
			if (this->m_Size > this->m_bufferSize)
			{
				if (this->m_Data)
				{
					free(this->m_Data);
				}

				this->m_Data = (double *)malloc(this->m_Size * sizeof(double));
				if (this->m_Data == nullptr)
				{
					std::string error = "Out of memory whilst attempting to resize a set of audio frames";
					OrchestratorBase::HandleException(error, OrchestratorExceptionType::MEMORY_ALLOCATION);
				}

				this->m_bufferSize = this->m_Size;
			}
		}

		void AudioFrames::Resize(size_t nFrames, unsigned int nChannels, double value)
		{
			this->Resize(nFrames, nChannels);
			for (size_t i = 0; i < this->m_Size; i++)
			{
				this->m_Data[i] = value;
			}
		}

		AudioFrames& AudioFrames::GetChannel(unsigned int sourceChannel, AudioFrames& destinationFrames, unsigned int destinationChannel) const
		{
			int srcStride = this->m_nChannels;
			int destStride = destinationFrames.m_nChannels;
			for (unsigned int i = sourceChannel, j = destinationChannel; i < this->m_nFrames * this->m_nChannels; i += srcStride, j += destStride)
			{
				destinationFrames[j] = this->m_Data[i];
			}

			return destinationFrames;

		}

		void AudioFrames::SetChannel(unsigned int destinationChannel, const Orchestrator::AudioFrames &sourceFrames, unsigned int sourceChannel)
		{
			unsigned int srcStride = sourceFrames.m_nChannels;
			unsigned int destStride = this->m_nChannels;
			for (unsigned int i = destinationChannel, j = sourceChannel; i < this->m_nFrames * this->m_nChannels; i += destStride, j += srcStride)
			{
				this->m_Data[i] = sourceFrames[j];
			}
		}

		double AudioFrames::Interpolate(double frame, unsigned int channel) const
		{
			size_t iIndex = (size_t)frame;
			double alpha = frame - (double)iIndex;
			double output;
			iIndex = iIndex * this->m_nChannels + channel;
			output = this->m_Data[iIndex];
			if (alpha > 0.0)
			{
				output += (alpha * (this->m_Data[iIndex + this->m_nChannels] - output));
			}

			return output;
		}
	}
}