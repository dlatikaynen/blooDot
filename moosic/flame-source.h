#pragma once

#include "oscillator-exception.h"
#include "oscillator-base.h"
#include "frame.h"

namespace blooDot::Oscillator
{
	class FlameSource : public OscillatorBase
	{
	public:
		FlameSource(void);
		unsigned int GetChannelsOut(void) const;
		const AudioFrame& GetLastFrame(void) const;

		virtual AudioFrame& Render(AudioFrame& frames, unsigned int channel = 0) = 0;

	protected:
		AudioFrame m_lastFrame;
	};
}