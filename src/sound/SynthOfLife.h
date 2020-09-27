#include "..\PreCompiledHeaders.h"
#include "SynthSequence.h"

#pragma once

namespace blooDot
{
	class SynthOfLife
	{
	public:
		SynthOfLife::SynthOfLife() { };
		SynthOfLife::SynthOfLife(const SynthOfLife& obj) { };
		SynthOfLife& operator=(const SynthOfLife& obj) { return *this; };
		SynthOfLife::SynthOfLife(SynthOfLife&& obj) { };
		SynthOfLife::~SynthOfLife();

		void Load(Platform::String^ fileName);
		std::shared_ptr<SynthSequence> Play(std::shared_ptr<Audio> audioEngine);
		void Save(Platform::String^ fileName);

	private:
		std::shared_ptr<PIECE_OF_MUSIC>		m_loadedSequence;
		std::shared_ptr<SynthSequence>		m_sequenceAsPlaying;
	};
}
