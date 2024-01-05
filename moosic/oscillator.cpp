#include "pch.h"
#include "oscillator.h"
#include <cmath>

#define BEND_MIDPOINT (8192.0)
#define INITIAL_TAGNUMBER (static_cast<long>(42))
#define HALFNOTES_PER_OCTAVE (12.0)
#define A3_OFFSET (220.0)
#define MIDINOTE_MAX (57.0)

namespace blooDot::Oscillator
{
	Oscillator::Oscillator(double decayTime)
	{
		this->m_lastFrame.Resize(1, 1, 0.0);
		this->m_muteTime = (int)(decayTime * OscillatorBase::SampleRate());
		this->m_Tags = INITIAL_TAGNUMBER;
	}

	void Oscillator::AddSynthesizer(Synthesizer* synth, int group)
	{
		Oscillator::SynthesizerSource synthSource;
		synthSource.synthesizer = synth;
		synthSource.group = group;
		synthSource.midiNoteNumber = -1;
		this->m_synthesizerSources.push_back(synthSource);
		if (synth->GetChannelsOut() > this->m_lastFrame.Channels())
		{
			unsigned int startChannel = this->m_lastFrame.Channels();
			this->m_lastFrame.Resize(1, synth->GetChannelsOut());
			for (unsigned int i = startChannel; i < this->m_lastFrame.Size(); i++)
			{
				this->m_lastFrame[i] = 0.0;
			}
		}
	}

	void Oscillator::RemoveSynthesizer(Synthesizer* synth)
	{
		bool found = false;
		std::vector<Oscillator::SynthesizerSource>::iterator i;
		for (i = this->m_synthesizerSources.begin(); i != this->m_synthesizerSources.end(); ++i)
		{
			if ((*i).synthesizer != synth)
			{
				continue;
			}

			this->m_synthesizerSources.erase(i);
			found = true;
			break;
		}

		if (found)
		{
			unsigned int maxChannels = 1;
			for (i = this->m_synthesizerSources.begin(); i != this->m_synthesizerSources.end(); ++i)
			{
				if ((*i).synthesizer->GetChannelsOut() > maxChannels)
				{
					maxChannels = (*i).synthesizer->GetChannelsOut();
				}
			}

			if (maxChannels < this->m_lastFrame.Channels())
			{
				this->m_lastFrame.Resize(1, maxChannels);
			}
		}
	}

	long Oscillator::Start(double midiNoteNumber, double amplitude, int group)
	{
		unsigned int i;
		double freq = (double)220.0 * ::pow(2.0, (midiNoteNumber - 57.0) / 12.0);
		for (i = 0; i < this->m_synthesizerSources.size(); i++)
		{
			if (this->m_synthesizerSources[i].midiNoteNumber < 0 && this->m_synthesizerSources[i].group == group)
			{
				this->m_synthesizerSources[i].tag = this->m_Tags++;
				this->m_synthesizerSources[i].group = group;
				this->m_synthesizerSources[i].midiNoteNumber = midiNoteNumber;
				this->m_synthesizerSources[i].freq = freq;
				this->m_synthesizerSources[i].synthesizer->Start(freq, amplitude * INVERSE_HALFBYTE);
				this->m_synthesizerSources[i].isSounding = 1;
				return this->m_synthesizerSources[i].tag;
			}
		}

		int sourceIndex = -1;
		for (i = 0; i < this->m_synthesizerSources.size(); i++)
		{
			if (this->m_synthesizerSources[i].group == group)
			{
				if (sourceIndex == -1)
				{
					sourceIndex = i;
				}
				else if (this->m_synthesizerSources[i].tag < this->m_synthesizerSources[sourceIndex].tag)
				{
					sourceIndex = static_cast<int>(i);
				}
			}
		}

		if (sourceIndex >= 0)
		{
			this->m_synthesizerSources[sourceIndex].tag = this->m_Tags++;
			this->m_synthesizerSources[sourceIndex].group = group;
			this->m_synthesizerSources[sourceIndex].midiNoteNumber = midiNoteNumber;
			this->m_synthesizerSources[sourceIndex].freq = freq;
			this->m_synthesizerSources[sourceIndex].synthesizer->Start(freq, amplitude * INVERSE_HALFBYTE);
			this->m_synthesizerSources[sourceIndex].isSounding = 1;
			return this->m_synthesizerSources[sourceIndex].tag;
		}

		return -1;
	}

	void Oscillator::Stop(double noteNumber, double amplitude, int group)
	{
		for (unsigned int i = 0; i < this->m_synthesizerSources.size(); i++)
		{
			if (this->m_synthesizerSources[i].midiNoteNumber == noteNumber && this->m_synthesizerSources[i].group == group)
			{
				this->m_synthesizerSources[i].synthesizer->Stop(amplitude * INVERSE_HALFBYTE);
				this->m_synthesizerSources[i].isSounding = -this->m_muteTime;
			}
		}
	}

	void Oscillator::Stop(long tag, double amplitude)
	{
		for (unsigned int i = 0; i < this->m_synthesizerSources.size(); ++i)
		{
			if (this->m_synthesizerSources[i].tag == tag)
			{
				this->m_synthesizerSources[i].synthesizer->Stop(amplitude * INVERSE_HALFBYTE);
				this->m_synthesizerSources[i].isSounding = -this->m_muteTime;
				break;
			}
		}
	}

	void Oscillator::SetFreq(double noteNumber, int group)
	{
		auto freq = static_cast<double>(A3_OFFSET) * ::pow(2.0, (noteNumber - MIDINOTE_MAX) / HALFNOTES_PER_OCTAVE);
		for (unsigned int i = 0; i < this->m_synthesizerSources.size(); i++)
		{
			if (this->m_synthesizerSources[i].group == group)
			{
				this->m_synthesizerSources[i].midiNoteNumber = noteNumber;
				this->m_synthesizerSources[i].freq = freq;
				this->m_synthesizerSources[i].synthesizer->SetFreq(freq);
			}
		}
	}

	void Oscillator::SetFreq(long tag, double noteNumber)
	{
		auto freq = static_cast<double>(A3_OFFSET) * ::pow(2.0, (noteNumber - MIDINOTE_MAX) / HALFNOTES_PER_OCTAVE);
		for (unsigned int i = 0; i < this->m_synthesizerSources.size(); i++)
		{
			if (this->m_synthesizerSources[i].tag == tag)
			{
				this->m_synthesizerSources[i].midiNoteNumber = noteNumber;
				this->m_synthesizerSources[i].freq = freq;
				this->m_synthesizerSources[i].synthesizer->SetFreq(freq);
				break;
			}
		}
	}

	void Oscillator::Bend(double value, int group)
	{
		double factor;
		if (value < BEND_MIDPOINT)
		{
			factor = ::pow(0.5, (BEND_MIDPOINT - value) / BEND_MIDPOINT);
		}
		else
		{
			factor = ::pow(2.0, (value - BEND_MIDPOINT) / BEND_MIDPOINT);
		}

		for (unsigned int i = 0; i < this->m_synthesizerSources.size(); ++i)
		{
			if (this->m_synthesizerSources[i].group == group)
			{
				this->m_synthesizerSources[i].synthesizer->SetFreq(static_cast<double>(factor * this->m_synthesizerSources[i].freq));
			}
		}
	}

	void Oscillator::Bend(long tag, double value)
	{
		double factor;
		if (value < BEND_MIDPOINT)
		{
			factor = ::pow(0.5, (BEND_MIDPOINT - value) / BEND_MIDPOINT);
		}
		else
		{
			factor = ::pow(2.0, (value - BEND_MIDPOINT) / BEND_MIDPOINT);
		}

		for (unsigned int i = 0; i < this->m_synthesizerSources.size(); ++i)
		{
			if (this->m_synthesizerSources[i].tag == tag)
			{
				this->m_synthesizerSources[i].synthesizer->SetFreq(static_cast<double>(factor * this->m_synthesizerSources[i].freq));
				break;
			}
		}
	}

	void Oscillator::SetCtrl(int number, double value, int group)
	{
		for (unsigned int i = 0; i < this->m_synthesizerSources.size(); ++i)
		{
			if (this->m_synthesizerSources[i].group == group)
			{
				this->m_synthesizerSources[i].synthesizer->SetCtrl(number, value);
			}
		}
	}

	void Oscillator::SetCtrl(long tag, int number, double value)
	{
		for (unsigned int i = 0; i < this->m_synthesizerSources.size(); ++i)
		{
			if (this->m_synthesizerSources[i].tag == tag)
			{
				this->m_synthesizerSources[i].synthesizer->SetCtrl(number, value);
				break;
			}
		}
	}

	void Oscillator::CutOut(void)
	{
		for (unsigned int i = 0; i < this->m_synthesizerSources.size(); ++i)
		{
			if (this->m_synthesizerSources[i].isSounding > 0)
			{
				this->m_synthesizerSources[i].synthesizer->Stop(0.5);
			}
		}
	}
}