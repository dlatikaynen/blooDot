#include "SynthPrimitive.h"
#include "SynthSequence.h"

using namespace blooDot;

SynthSequence::SynthSequence(std::shared_ptr<Audio> audioEngine)
{
	this->m_audioEngine = audioEngine;
}

SynthSequence::~SynthSequence()
{

}

void SynthSequence::Source(std::shared_ptr<PIECE_OF_MUSIC> entirePiece)
{
	this->m_entirePiece = entirePiece;
}

void SynthSequence::Update(float timeTotal, float timeDelta)
{
	if (this->m_timeStarted == 0.f)
	{
		if (timeTotal == 0.f)
		{
			return;
		}

		this->m_timeStarted = timeTotal;
	}

	//m_currentlyPlaying
}

void SynthSequence::Render()
{
	//this->m_audioEngine->
}
