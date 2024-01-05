#pragma once

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

namespace blooDot::Oscillator
{
	class AudioFrame
	{
	public:
		AudioFrame(unsigned int nFrames = 0, unsigned int nChannels = 0);
		AudioFrame(const double& value, unsigned int nFrames, unsigned int nChannels);
		~AudioFrame();
		AudioFrame(const AudioFrame& f);

		AudioFrame& operator= (const AudioFrame& f);
		double& operator[] (size_t n);
		double operator[] (size_t n) const;
		AudioFrame operator+(const AudioFrame& frames) const;
		void operator+= (AudioFrame& f);
		void operator*= (AudioFrame& f);
		double& operator() (size_t frame, unsigned int channel);
		double operator() (size_t frame, unsigned int channel) const;

		double Interpolate(double frame, unsigned int channel = 0) const;
		size_t Size() const { return this->m_Size; };
		bool Empty() const;
		void Resize(size_t nFrames, unsigned int nChannels = 1);
		void Resize(size_t nFrames, unsigned int nChannels, double value);
		AudioFrame& GetChannel(unsigned int channel, AudioFrame& destinationFrames, unsigned int destinationChannel) const;
		void SetChannel(unsigned int channel, const AudioFrame& sourceFrames, unsigned int sourceChannel);
		unsigned int Channels(void) const;
		unsigned int Frames(void) const;
		void SetDataRate(double rate);
		double DataRate(void) const;

	private:
		double* m_Data;
		double			m_dataRate;
		size_t			m_nFrames;
		unsigned int	m_nChannels;
		size_t			m_Size;
		size_t			m_bufferSize;
	};

	inline bool AudioFrame::Empty() const
	{
		return this->m_Size == 0;
	}

	inline double& AudioFrame :: operator[] (size_t n)
	{
		return this->m_Data[n];
	}

	inline double AudioFrame :: operator[] (size_t n) const
	{
		return this->m_Data[n];
	}

	inline double& AudioFrame :: operator() (size_t frame, unsigned int channel)
	{
		return this->m_Data[frame * this->m_nChannels + channel];
	}

	inline double AudioFrame :: operator() (size_t frame, unsigned int channel) const
	{
		return this->m_Data[frame * this->m_nChannels + channel];
	}

	inline AudioFrame AudioFrame::operator+(const AudioFrame& f) const
	{
		AudioFrame sum((unsigned int)this->m_nFrames, this->m_nChannels);
		
		double* sumPtr = &sum[0];
		const double* fptr = f.m_Data;
		const double* dPtr = this->m_Data;

		for (unsigned int i = 0; i < this->m_Size; i++)
		{
			*sumPtr++ = *fptr++ + *dPtr++;
		}

		return sum;
	}

	inline void AudioFrame :: operator+= (AudioFrame& f)
	{
		double* fptr = &f[0];
		double* dptr = this->m_Data;

		for (unsigned int i = 0; i < this->m_Size; i++)
		{
			*dptr++ += *fptr++;
		}
	}

	inline void AudioFrame :: operator*= (AudioFrame& f)
	{
		double* fptr = &f[0];
		double* dptr = this->m_Data;

		for (unsigned int i = 0; i < this->m_Size; i++)
		{
			*dptr++ *= *fptr++;
		}
	}
}