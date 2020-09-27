#include "..\PreCompiledHeaders.h"

#pragma once

namespace blooDot
{
	class SynthOfLife
	{
	public:
		void Load(Platform::String^ fileName);
		std::shared_ptr<SynthSequence> Play(std::shared_ptr<Audio> audioEngine);
		void Save(Platform::String^ fileName);

	private:
		std::shared_ptr<PIECE_OF_MUSIC>		m_loadedSequence;
		std::shared_ptr<SynthSequence>		m_sequenceAsPlaying;
	};
}