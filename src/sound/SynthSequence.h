#include "..\PreCompiledHeaders.h"
#include "Audio.h"
#include "orchester/Orchestrator.h"

#pragma once

namespace blooDot
{
	typedef uint32 TICK;
	class SynthPrimitive;

	struct PLAYABLE_ATOM_STRUCT
	{
		TICK StartOffset;
		std::shared_ptr<SynthPrimitive> Note;
		TICK Sustain;
	};

	typedef std::shared_ptr<PLAYABLE_ATOM_STRUCT> PLAYABLE_ATOM;

	struct BAR_OF_MUSIC_STRUCT
	{
		TICK StartOffset;
		std::vector<PLAYABLE_ATOM> Notes;
	};

	typedef blooDot::BAR_OF_MUSIC_STRUCT BAR_OF_MUSIC;
	typedef std::vector<BAR_OF_MUSIC> PIECE_OF_MUSIC;

	class SynthSequence
	{
	public:
		SynthSequence::SynthSequence(std::shared_ptr<Audio>);

		SynthSequence::SynthSequence(const SynthSequence& obj) { };
		SynthSequence& operator=(const SynthSequence& obj) { return *this; };
		SynthSequence::SynthSequence() { };
		SynthSequence::SynthSequence(SynthSequence&& obj) { };
		SynthSequence::~SynthSequence();

		void Source(std::shared_ptr<PIECE_OF_MUSIC> entirePiece);
		void Update(float timeTotal, float timeDelta);
		void Render();
		bool GetNextBuffer(uint8* buffer, uint32 maxBufferSize, uint32* bufferLength);

	private:
		std::shared_ptr<Audio>			m_audioEngine;
		std::shared_ptr<PIECE_OF_MUSIC>	m_entirePiece;
		PIECE_OF_MUSIC					m_currentlyPlaying;
		int								m_minBarIndex;
		float							m_timeStarted;
		Orchestrator::Orchestrator*		m_Orchestrator;
		std::vector<int>				m_Melody;
		std::vector<int>::iterator		m_Note;
	};
}