#pragma once

#include "synth.h"
#include <vector>

namespace blooDot::Oscillator
{
	class Oscillator : public OscillatorBase
	{
	public:
		Oscillator(double decayTime = 0.2); /* seconds */

		void AddSynthesizer(Synthesizer* instrument, int group = 0);
		void RemoveSynthesizer(Synthesizer* instrument);
		long Start(double noteNumber, double amplitude, int group = 0);
		void Stop(double noteNumber, double amplitude, int group = 0);
		void Stop(long tag, double amplitude);
		void SetCtrl(int number, double value, int group = 0);
		void SetCtrl(long tag, int number, double value);
		void SetFreq(double noteNumber, int group = 0);
		void SetFreq(long tag, double noteNumber);
		void Bend(double value, int group = 0);
		void Bend(long tag, double value);
		void CutOut(void);
		unsigned int SetChannelsOut(void) const { return this->m_lastFrame.Channels(); };
		const AudioFrame& LastFrame(void) const { return this->m_lastFrame; };
		double LastOut(unsigned int channel = 0);
		double Render(unsigned int channel = 0);
		AudioFrame& Render(AudioFrame& frames, unsigned int channel = 0);

	protected:
		struct SynthesizerSource
		{
			Synthesizer* synthesizer;
			long tag;
			double midiNoteNumber;
			double freq;
			int isSounding;
			int group;

			struct SynthesizerSource() :
				synthesizer(0),
				tag(0),
				midiNoteNumber(-1.0),
				freq(0.0),
				isSounding(0),
				group(0)
			{ }
		};

	private:
		std::vector<SynthesizerSource> m_synthesizerSources;
		AudioFrame m_lastFrame;
		int m_muteTime;
		long m_Tags;
	};

	inline double Oscillator::LastOut(unsigned int channel)
	{
		return this->m_lastFrame[channel];
	}

	inline double Oscillator::Render(unsigned int channel)
	{
		unsigned int j;
		for (j = 0; j < this->m_lastFrame.Channels(); ++j)
		{
			this->m_lastFrame[j] = 0.0;
		}

		for (unsigned int i = 0; i < this->m_synthesizerSources.size(); ++i)
		{
			if (this->m_synthesizerSources[i].isSounding != 0)
			{
				this->m_synthesizerSources[i].synthesizer->Render();
				for (j = 0; j < this->m_synthesizerSources[i].synthesizer->GetChannelsOut(); j++)
				{
					this->m_lastFrame[j] += this->m_synthesizerSources[i].synthesizer->LastOut(j);
				}
			}

			if (this->m_synthesizerSources[i].isSounding < 0)
			{
				this->m_synthesizerSources[i].isSounding++;
			}

			if (this->m_synthesizerSources[i].isSounding == 0)
			{
				this->m_synthesizerSources[i].midiNoteNumber = -1;
			}
		}

		return this->m_lastFrame[channel];
	}

	inline AudioFrame& Oscillator::Render(AudioFrame& frames, unsigned int channel)
	{
		unsigned int nChannels = this->m_lastFrame.Channels();
		double* samples = &frames[channel];
		unsigned int j, stride = frames.Channels() - nChannels;
		for (unsigned int i = 0; i < frames.Frames(); i++, samples += stride)
		{
			this->Render();
			for (j = 0; j < nChannels; j++)
			{
				*samples++ = this->m_lastFrame[j];
			}
		}

		return frames;
	}
}