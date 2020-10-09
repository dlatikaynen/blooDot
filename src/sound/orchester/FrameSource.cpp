#include "FrameSource.h"

namespace blooDot
{
	namespace Orchestrator
	{
		FrameSource::FrameSource(void)
		{
			this->m_lastFrame.Resize(1, 1, 0.);
		}

		unsigned int FrameSource::GetChannelsOut(void) const
		{
			return this->m_lastFrame.Channels();
		}
		
		const AudioFrames& FrameSource::GetLastFrame(void) const
		{
			return this->m_lastFrame;
		}
	}
}