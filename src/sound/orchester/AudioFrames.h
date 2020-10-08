#pragma once

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

namespace blooDot
{
	namespace Orchestrator
	{
		class AudioFrames
		{
		public:
			AudioFrames(unsigned int nFrames = 0, unsigned int nChannels = 0);
			AudioFrames(const double& value, unsigned int nFrames, unsigned int nChannels);
			~AudioFrames();
			AudioFrames(const AudioFrames& f);

			AudioFrames& operator= (const AudioFrames& f);
			double& operator[] (size_t n);
			double operator[] (size_t n) const;
			AudioFrames operator+(const AudioFrames &frames) const;
			void operator+= (AudioFrames& f);
			void operator*= (AudioFrames& f);
			double& operator() (size_t frame, unsigned int channel);
			double operator() (size_t frame, unsigned int channel) const;

			double Interpolate(double frame, unsigned int channel = 0) const;
			size_t Size() const { return this->m_Size; };
			bool Empty() const;
			void Resize(size_t nFrames, unsigned int nChannels = 1);
			void Resize(size_t nFrames, unsigned int nChannels, double value);
			AudioFrames& GetChannel(unsigned int channel, AudioFrames& destinationFrames, unsigned int destinationChannel) const;
			void SetChannel(unsigned int channel, const AudioFrames &sourceFrames, unsigned int sourceChannel);
			unsigned int Channels(void) const { return this->m_nChannels; };
			unsigned int Frames(void) const { return (unsigned int)this->m_nFrames; };
			void SetDataRate(double rate) { this->m_dataRate = rate; };
			double DataRate(void) const { return this->m_dataRate; };

		private:
			double *m_Data;
			double m_dataRate;
			size_t m_nFrames;
			unsigned int m_nChannels;
			size_t m_Size;
			size_t m_bufferSize;
		};

		inline bool AudioFrames::Empty() const
		{
			return this->m_Size == 0;
		}

		inline double& AudioFrames :: operator[] (size_t n)
		{
			return this->m_Data[n];
		}

		inline double AudioFrames :: operator[] (size_t n) const
		{
			return this->m_Data[n];
		}

		inline double& AudioFrames :: operator() (size_t frame, unsigned int channel)
		{
			return this->m_Data[frame * this->m_nChannels + channel];
		}

		inline double AudioFrames :: operator() (size_t frame, unsigned int channel) const
		{
			return this->m_Data[frame * this->m_nChannels + channel];
		}

		inline AudioFrames AudioFrames::operator+(const AudioFrames &f) const
		{
			AudioFrames sum((unsigned int)this->m_nFrames, this->m_nChannels);
			double *sumPtr = &sum[0];
			const double *fptr = f.m_Data;
			const double *dPtr = this->m_Data;
			for (unsigned int i = 0; i < this->m_Size; i++)
			{
				*sumPtr++ = *fptr++ + *dPtr++;
			}

			return sum;
		}

		inline void AudioFrames :: operator+= (AudioFrames& f)
		{
			double *fptr = &f[0];
			double *dptr = this->m_Data;
			for (unsigned int i = 0; i < this->m_Size; i++)
			{
				*dptr++ += *fptr++;
			}
		}

		inline void AudioFrames :: operator*= (AudioFrames& f)
		{
			double *fptr = &f[0];
			double *dptr = this->m_Data;
			for (unsigned int i = 0; i < this->m_Size; i++)
			{
				*dptr++ *= *fptr++;
			}
		}
	}
}