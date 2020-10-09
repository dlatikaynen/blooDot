#pragma once

#include "OrchestratorException.h"
#include "OrchestratorBase.h"
#include "AudioFrames.h"

namespace blooDot
{
	namespace Orchestrator
	{
		class FrameSource : public OrchestratorBase
		{
		public:
			FrameSource(void);
			unsigned int GetChannelsOut(void) const;
			const AudioFrames& GetLastFrame(void) const;
			
			virtual AudioFrames& Render(AudioFrames& frames, unsigned int channel = 0) = 0;

		protected:
			AudioFrames m_lastFrame;
		};
	}
}
