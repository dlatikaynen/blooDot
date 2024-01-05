#include "pch.h"
#include "flame-source.h"

namespace blooDot::Oscillator
{
	FlameSource::FlameSource(void)
	{
		this->m_lastFrame.Resize(1, 1, 0.);
	}

	unsigned int FlameSource::GetChannelsOut(void) const
	{
		return this->m_lastFrame.Channels();
	}

	const AudioFrame& FlameSource::GetLastFrame(void) const
	{
		return this->m_lastFrame;
	}
}