#include "SynthPrimitive.h"
#include "SynthSequence.h"
#include "..\dx\DirectXHelper.h"

using namespace blooDot;

SynthSequence::SynthSequence(std::shared_ptr<Audio> audioEngine)
{
	this->m_audioEngine = audioEngine;
	this->m_Orchestrator = nullptr;
	this->m_Pluckable = nullptr;
	this->m_Melody.assign({ 15, 18, 20, 22, 20 });
}

SynthSequence::~SynthSequence()
{
	delete this->m_Orchestrator;
	delete this->m_Pluckable;
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

	if (this->m_Orchestrator == nullptr)
	{
		this->m_Orchestrator = new blooDot::Orchestrator::Orchestrator(0.1);
		this->m_Orchestrator->SetSampleRate(22050);
		this->m_Pluckable = new blooDot::Orchestrator::StringShape();
		this->m_Orchestrator->AddSynthesizer(this->m_Pluckable);
		this->m_Orchestrator->Start(static_cast<float>(25), 100.f);
		this->m_Note = this->m_Melody.begin();
	}

	//m_currentlyPlaying
}

void SynthSequence::Render()
{
	bool streamComplete;
	XAUDIO2_VOICE_STATE state;
	uint32 bufferLength;
	XAUDIO2_BUFFER buf = { 0 };

	this->m_audioEngine->PlaySynth();
	this->m_audioEngine->m_synthSourceVoice->GetState(&state);
	while (state.BuffersQueued <= MAX_BUFFER_COUNT - 1)
	{
		streamComplete = this->GetNextBuffer
		(
			this->m_audioEngine->m_synthBuffers[this->m_audioEngine->m_currentSynthBuffer],
			STREAMING_BUFFER_SIZE,
			&bufferLength
		);

		if (bufferLength > 0)
		{
			buf.AudioBytes = bufferLength;
			buf.pAudioData = this->m_audioEngine->m_synthBuffers[this->m_audioEngine->m_currentSynthBuffer];
			buf.Flags = streamComplete ? XAUDIO2_END_OF_STREAM : 0;
			buf.pContext = 0;
			DX::ThrowIfFailed
			(
				this->m_audioEngine->m_synthSourceVoice->SubmitSourceBuffer(&buf)
			);

			this->m_audioEngine->m_currentSynthBuffer++;
			this->m_audioEngine->m_currentSynthBuffer %= MAX_BUFFER_COUNT;
		}

		if (streamComplete)
		{
			this->m_audioEngine->m_synthSourceVoice->Stop();
			break;
		}

		this->m_audioEngine->m_synthSourceVoice->GetState(&state);
	}
}

bool toggle = false;

bool SynthSequence::GetNextBuffer(uint8* buffer, uint32 maxBufferSize, uint32* bufferLength)
{
	register float sample;

	this->m_Orchestrator->Start(static_cast<float>(*this->m_Note), 100.f);
	++this->m_Note;
	if (this->m_Note == this->m_Melody.end())
	{
		this->m_Note = this->m_Melody.begin();
	}

	for (unsigned i = 0; i < maxBufferSize; ++i)
	{
		sample = static_cast<float>(100. * this->m_Orchestrator->Render());
		buffer[i] = static_cast<uint8>(sample);
	}

	this->m_Orchestrator->CutOut();

	*bufferLength = maxBufferSize;
	return false;
}
