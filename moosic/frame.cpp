#include "pch.h"
#include "oscillator-exception.h"
#include "oscillator-base.h"
#include "frame.h"

namespace blooDot::Oscillator
{
	AudioFrame::AudioFrame(unsigned int nFrames, unsigned int nChannels) :
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
				OscillatorBase::HandleException(error, OrchestratorExceptionType::MEMORY_ALLOCATION);
			}
		}

		this->m_dataRate = OscillatorBase::SampleRate();
	}

	AudioFrame::AudioFrame(const double& value, unsigned int nFrames, unsigned int nChannels) :
		m_Data(0),
		m_nFrames(nFrames),
		m_nChannels(nChannels)
	{
		this->m_Size = this->m_nFrames * this->m_nChannels;
		this->m_bufferSize = this->m_Size;
		if (this->m_Size > 0)
		{
			this->m_Data = (double*)malloc(this->m_Size * sizeof(double));
			if (this->m_Data == NULL)
			{
				std::string error = "Out of memory in audio frame storage allocation";
				OscillatorBase::HandleException(error, OrchestratorExceptionType::MEMORY_ALLOCATION);
			}

			for (long i = 0; i < (long)this->m_Size; i++)
			{
				this->m_Data[i] = value;
			}
		}

		this->m_dataRate = OscillatorBase::SampleRate();
	}

	AudioFrame::~AudioFrame()
	{
		if (this->m_Data)
		{
			free(this->m_Data);
		}
	}

	AudioFrame::AudioFrame(const AudioFrame& f) :
		m_Data(0),
		m_Size(0),
		m_bufferSize(0)
	{
		this->Resize(f.Frames(), f.Channels());
		this->m_dataRate = OscillatorBase::SampleRate();
		for (unsigned int i = 0; i < this->m_Size; i++)
		{
			this->m_Data[i] = f[i];
		}
	}

	AudioFrame& AudioFrame :: operator= (const AudioFrame& f)
	{
		if (this->m_Data)
		{
			free(this->m_Data);
		}

		this->m_Data = 0;
		this->m_Size = 0;
		this->m_bufferSize = 0;
		Resize(f.Frames(), f.Channels());
		this->m_dataRate = OscillatorBase::SampleRate();
		for (unsigned int i = 0; i < this->m_Size; i++)
		{
			this->m_Data[i] = f[i];
		}

		return *this;
	}

	void AudioFrame::Resize(size_t nFrames, unsigned int nChannels)
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

			this->m_Data = (double*)malloc(this->m_Size * sizeof(double));
			if (this->m_Data == nullptr)
			{
				std::string error = "Out of memory whilst attempting to resize a set of audio frames";
				OscillatorBase::HandleException(error, OrchestratorExceptionType::MEMORY_ALLOCATION);
			}

			this->m_bufferSize = this->m_Size;
		}
	}

	void AudioFrame::Resize(size_t nFrames, unsigned int nChannels, double value)
	{
		this->Resize(nFrames, nChannels);
		for (size_t i = 0; i < this->m_Size; i++)
		{
			this->m_Data[i] = value;
		}
	}

	AudioFrame& AudioFrame::GetChannel(unsigned int sourceChannel, AudioFrame& destinationFrames, unsigned int destinationChannel) const
	{
		int srcStride = this->m_nChannels;
		int destStride = destinationFrames.m_nChannels;
		for (unsigned int i = sourceChannel, j = destinationChannel; i < this->m_nFrames * this->m_nChannels; i += srcStride, j += destStride)
		{
			destinationFrames[j] = this->m_Data[i];
		}

		return destinationFrames;

	}

	void AudioFrame::SetChannel(unsigned int destinationChannel, const Oscillator::AudioFrame& sourceFrames, unsigned int sourceChannel)
	{
		unsigned int srcStride = sourceFrames.m_nChannels;
		unsigned int destStride = this->m_nChannels;
		for (unsigned int i = destinationChannel, j = sourceChannel; i < this->m_nFrames * this->m_nChannels; i += destStride, j += srcStride)
		{
			this->m_Data[i] = sourceFrames[j];
		}
	}

	double AudioFrame::Interpolate(double frame, unsigned int channel) const
	{
		size_t iIndex = (size_t)frame;
		double alpha = frame - (double)iIndex;
		double output;
		iIndex = iIndex * this->m_nChannels + channel;
		output = this->m_Data[iIndex];
		if (alpha > 0.)
		{
			output += (alpha * (this->m_Data[iIndex + this->m_nChannels] - output));
		}

		return output;
	}

	unsigned int AudioFrame::Channels(void) const
	{
		return this->m_nChannels;
	}

	unsigned int AudioFrame::Frames(void) const
	{
		return (unsigned int)this->m_nFrames;
	}

	void AudioFrame::SetDataRate(double rate)
	{
		this->m_dataRate = rate;
	}

	double AudioFrame::DataRate(void) const
	{
		return this->m_dataRate;
	}
}