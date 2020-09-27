#include "..\PreCompiledHeaders.h"
#include "Audio.h"

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

		void Source(std::shared_ptr<PIECE_OF_MUSIC> entirePiece);
		void Update(float timeTotal, float timeDelta);
		void Render();

	private:
		std::shared_ptr<Audio>			m_audioEngine;
		std::shared_ptr<PIECE_OF_MUSIC>	m_entirePiece;
		PIECE_OF_MUSIC					m_currentlyPlaying;
		int								m_minBarIndex;
		float							m_timeStarted;
	};
}