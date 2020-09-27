#include "SynthSequence.h"
#include "SynthOfLife.h"

using namespace blooDot;

void SynthOfLife::Load(Platform::String^ fileName)
{
	this->m_loadedSequence = std::make_shared<PIECE_OF_MUSIC>();


}

std::shared_ptr<SynthSequence> SynthOfLife::Play(std::shared_ptr<Audio> audioEngine)
{
	this->m_sequenceAsPlaying = std::make_shared<SynthSequence>(audioEngine);
	this->m_sequenceAsPlaying->Source(this->m_loadedSequence);
	return this->m_sequenceAsPlaying;
}

void SynthOfLife::Save(Platform::String^ fileName)
{
	if (this->m_loadedSequence != nullptr)
	{


	}
}
