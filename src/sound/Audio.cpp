#include "..\PreCompiledHeaders.h"
#include "..\dx\DirectXHelper.h"
#include "Audio.h"

void AudioEngineCallbacks::Initialize(Audio* audio)
{
    m_audio = audio;
}

// Called when a critical system error causes XAudio2
// to be closed and restarted. The error code is given in Error.
void  _stdcall AudioEngineCallbacks::OnCriticalError(HRESULT Error)
{
    m_audio->SetEngineExperiencedCriticalError();
}

Audio::Audio()
{
	this->m_currentBuffer = 0;
	this->m_engineExperiencedCriticalError = false;
	this->m_isMusicStarted = false;
	this->m_isSfxStarted = false;
	this->m_isMusicPaused = false;
	this->m_isSfxPaused = false;
	this->m_musicEngine = nullptr;
	this->m_soundEffectEngine = nullptr;
	this->m_musicMasteringVoice = nullptr;
	this->m_soundEffectMasteringVoice = nullptr;
	this->m_musicSourceVoice = nullptr;
	this->m_soundEffectReverbVoiceSmallRoom = nullptr;
	this->m_soundEffectReverbVoiceLargeRoom = nullptr;
	this->m_musicReverbVoiceSmallRoom = nullptr;
	this->m_musicReverbVoiceLargeRoom = nullptr;
}

Audio::~Audio()
{
	this->SuspendSfx();
	this->SuspendMusic();
}

void Audio::Initialize()
{
    m_isMusicStarted = false;
	m_isSfxStarted = false;
	m_musicEngine = nullptr;
    m_soundEffectEngine = nullptr;
    m_musicMasteringVoice = nullptr;
    m_soundEffectMasteringVoice = nullptr;
    m_musicSourceVoice = nullptr;

    m_reverbParametersSmall.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
    m_reverbParametersSmall.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
    m_reverbParametersSmall.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
    m_reverbParametersSmall.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
    m_reverbParametersSmall.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
    m_reverbParametersSmall.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
    m_reverbParametersSmall.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
    m_reverbParametersSmall.EarlyDiffusion = 4;
    m_reverbParametersSmall.LateDiffusion = 15;
    m_reverbParametersSmall.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
    m_reverbParametersSmall.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
    m_reverbParametersSmall.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
    m_reverbParametersSmall.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
    m_reverbParametersSmall.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
    m_reverbParametersSmall.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
    m_reverbParametersSmall.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
    m_reverbParametersSmall.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
    m_reverbParametersSmall.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
    m_reverbParametersSmall.DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
    m_reverbParametersSmall.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
    m_reverbParametersSmall.RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
    m_reverbParametersSmall.WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;
    m_reverbParametersSmall.DisableLateField = TRUE;

    m_reverbParametersLarge.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
    m_reverbParametersLarge.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
    m_reverbParametersLarge.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
    m_reverbParametersLarge.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
    m_reverbParametersLarge.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
    m_reverbParametersLarge.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
    m_reverbParametersLarge.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
    m_reverbParametersLarge.EarlyDiffusion = 4;
    m_reverbParametersLarge.LateDiffusion = 4;
    m_reverbParametersLarge.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
    m_reverbParametersLarge.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
    m_reverbParametersLarge.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
    m_reverbParametersLarge.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
    m_reverbParametersLarge.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
    m_reverbParametersLarge.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
    m_reverbParametersLarge.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
    m_reverbParametersLarge.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
    m_reverbParametersLarge.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
    m_reverbParametersLarge.DecayTime = 1.0f;
    m_reverbParametersLarge.Density = 10;
    m_reverbParametersLarge.RoomSize = 100;
    m_reverbParametersLarge.WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;
    m_reverbParametersLarge.DisableLateField = FALSE;
    for (int i = 0; i < ARRAYSIZE(m_soundEffects); i++)
    {
        m_soundEffects[i].m_soundEffectBufferData = nullptr;
        m_soundEffects[i].m_soundEffectSourceVoice = nullptr;
        m_soundEffects[i].m_soundEffectStarted = false;
        ZeroMemory(&m_soundEffects[i].m_audioBuffer, sizeof(m_soundEffects[i].m_audioBuffer));
    }

    ZeroMemory(m_audioBuffers, sizeof(m_audioBuffers));
}

void Audio::SetRoomSize(float roomSize, float* wallDistances)
{
    // Maximum "outdoors" is a room size of 1000.0f
    float normalizedRoomSize = roomSize  / 1000.0f;
    float outputMatrix[4] = {0, 0, 0, 0};
    float leftRatio = wallDistances[2] / (wallDistances[2] + wallDistances[3]);
    if ((wallDistances[2] + wallDistances[3]) == 0)
    {
        leftRatio = 0.5f;
    }
    float width = abs((wallDistances[3] + wallDistances[5] + wallDistances[7]) / 3.0f - (wallDistances[2] + wallDistances[4] + wallDistances[6]) / 3.0f);
    float height = abs(wallDistances[1] - wallDistances[0]);
    float widthNormalized = width / max(width, height);
    if (width == 0)
    {
        widthNormalized = 1.0f;
    }

    // As widthNormalized approaches 0, the room is taller than wide.
    // This means that the late-field reverb should be non-localized across both speakers.
    leftRatio -= .5f;
    leftRatio *= widthNormalized;
    leftRatio += .5f;

    // Near-field reverb with the speaker closest to the wall getting more reverb
    outputMatrix[0] = (1.0f - leftRatio)* normalizedRoomSize;
    outputMatrix[1] = 0;
    outputMatrix[2] = 0;
    outputMatrix[3] = leftRatio * normalizedRoomSize;
    DX::ThrowIfFailed(
        m_soundEffectReverbVoiceSmallRoom->SetOutputMatrix(m_soundEffectMasteringVoice, 2, 2, outputMatrix)
    );

    outputMatrix[0] = (1.0f - leftRatio) * normalizedRoomSize;
    outputMatrix[1] = 0;
    outputMatrix[2] = 0;
    outputMatrix[3]= leftRatio * normalizedRoomSize;
    DX::ThrowIfFailed(
        m_musicReverbVoiceSmallRoom->SetOutputMatrix(m_musicMasteringVoice, 2, 2, outputMatrix)
    );

    // Late-field reverb: The wall furthest away gets more of the late-field reverb
    outputMatrix[0] = leftRatio * (1.0f - normalizedRoomSize);
    outputMatrix[1] = 0;
    outputMatrix[2] = 0;
    outputMatrix[3]= (1.0f - leftRatio) * (1.0f - normalizedRoomSize);
    DX::ThrowIfFailed(
        m_soundEffectReverbVoiceLargeRoom->SetOutputMatrix(m_soundEffectMasteringVoice, 2, 2, outputMatrix)
    );

    outputMatrix[0] = leftRatio * (1.0f - normalizedRoomSize);
    outputMatrix[1] = 0;
    outputMatrix[2] = 0;
    outputMatrix[3] = (1.0f - leftRatio) * (1.0f - normalizedRoomSize);
    DX::ThrowIfFailed(
        m_musicReverbVoiceLargeRoom->SetOutputMatrix(m_musicMasteringVoice, 2, 2, outputMatrix)
    );
}

void Audio::CreateResources()
{
    try
    {
        /* get both file I/O and format decode for audio assets.
		 * can replace streamer with own file I/O and decode routines */
		this->m_musicStreamer.Initialize(L"Media\\Audio\\background.mp3");
        DX::ThrowIfFailed(
            XAudio2Create(&this->m_musicEngine)
        );

#if defined(_DEBUG)
        XAUDIO2_DEBUG_CONFIGURATION debugConfig = {0};
        debugConfig.BreakMask = XAUDIO2_LOG_ERRORS;
        debugConfig.TraceMask = XAUDIO2_LOG_ERRORS;
		this->m_musicEngine->SetDebugConfiguration(&debugConfig);
#endif

		this->m_musicEngineCallback.Initialize(this);
		this->m_musicEngine->RegisterForCallbacks(&m_musicEngineCallback);

        // This sample plays the equivalent of background music, which we tag on the
        // mastering voice as AudioCategory_GameMedia. In ordinary usage, if we were
        // playing the music track with no effects, we could route it entirely through
        // Media Foundation. Here we are using XAudio2 to apply a reverb effect to the
        // music, so we use Media Foundation to decode the data then we feed it through
        // the XAudio2 pipeline as a separate Mastering Voice, so that we can tag it
        // as Game Media. We default the mastering voice to 2 channels to simplify
        // the reverb logic.
        DX::ThrowIfFailed(
			this->m_musicEngine->CreateMasteringVoice(
                &this->m_musicMasteringVoice,
                2,
                48000,
                0,
                nullptr,
                nullptr,
                AudioCategory_GameMedia
            )
        );

		this->CreateReverb(
			this->m_musicEngine,
			this->m_musicMasteringVoice,
            &this->m_reverbParametersSmall,
            &this->m_musicReverbVoiceSmallRoom,
            true
        );
        
		this->CreateReverb(
			this->m_musicEngine,
			this->m_musicMasteringVoice,
            &this->m_reverbParametersLarge,
            &this->m_musicReverbVoiceLargeRoom,
            true
        );

        XAUDIO2_SEND_DESCRIPTOR descriptors[3];
        descriptors[0].pOutputVoice = this->m_musicMasteringVoice;
        descriptors[0].Flags = 0;
        descriptors[1].pOutputVoice = this->m_musicReverbVoiceSmallRoom;
        descriptors[1].Flags = 0;
        descriptors[2].pOutputVoice = this->m_musicReverbVoiceLargeRoom;
        descriptors[2].Flags = 0;
        XAUDIO2_VOICE_SENDS sends = {0};
        sends.SendCount = 3;
        sends.pSends = descriptors;
        WAVEFORMATEX& waveFormat = this->m_musicStreamer.GetOutputWaveFormatEx();
        DX::ThrowIfFailed
		(
			this->m_musicEngine->CreateSourceVoice(
				&this->m_musicSourceVoice,
				&waveFormat,
				0,
				1.0f,
				&this->m_voiceContext,
				&sends, 
				nullptr
			)
        );

        DX::ThrowIfFailed
		(
			this->m_musicMasteringVoice->SetVolume(0.4f)
        );

        // Create a separate engine and mastering voice for sound effects in the sample
        // Games will use many voices in a complex graph for audio, mixing all effects down to a
        // single mastering voice.
        // We are creating an entirely new engine instance and mastering voice in order to tag
        // our sound effects with the audio category AudioCategory_GameEffects.
        DX::ThrowIfFailed
		(
            XAudio2Create(&this->m_soundEffectEngine)
        );

		this->m_soundEffectEngineCallback.Initialize(this);
		this->m_soundEffectEngine->RegisterForCallbacks(&this->m_soundEffectEngineCallback);

        /* we default the mastering voice to 2 channels to simplify the reverb logic */
        DX::ThrowIfFailed
		(
            this->m_soundEffectEngine->CreateMasteringVoice(&m_soundEffectMasteringVoice, 2, 48000, 0, nullptr, nullptr, AudioCategory_GameEffects)
        );

		this->CreateReverb(
			this->m_soundEffectEngine,
			this->m_soundEffectMasteringVoice,
			&this->m_reverbParametersSmall, 
			&this->m_soundEffectReverbVoiceSmallRoom,
			true
		);

		this->CreateReverb(
			this->m_soundEffectEngine,
			this->m_soundEffectMasteringVoice,
			&this->m_reverbParametersLarge,
			&this->m_soundEffectReverbVoiceLargeRoom,
			true
		);

		for (int eventId = SoundEvent::RollingEvent; eventId != SoundEvent::LastSoundEvent; ++eventId)
		{
			this->CreateSourceVoice(static_cast<SoundEvent>(eventId));
		}
	}
    catch (...)
    {
		this->m_engineExperiencedCriticalError = true;
    }
}

void Audio::CreateReverb(IXAudio2* engine, IXAudio2MasteringVoice* masteringVoice, XAUDIO2FX_REVERB_PARAMETERS* parameters, IXAudio2SubmixVoice** newSubmix, bool enableEffect)
{
    XAUDIO2_EFFECT_DESCRIPTOR soundEffectdescriptor;
    XAUDIO2_EFFECT_CHAIN soundEffectChain;
    Microsoft::WRL::ComPtr<IUnknown> soundEffectXAPO;
    DX::ThrowIfFailed(
        XAudio2CreateReverb(&soundEffectXAPO)
    );

    soundEffectdescriptor.InitialState = false;
    soundEffectdescriptor.OutputChannels = 2;
    soundEffectdescriptor.pEffect = soundEffectXAPO.Get();
    soundEffectChain.EffectCount = 1;
    soundEffectChain.pEffectDescriptors = &soundEffectdescriptor;
    DX::ThrowIfFailed(
        engine->CreateSubmixVoice(newSubmix, 2, 48000, 0, 0, nullptr, &soundEffectChain)
    );

    DX::ThrowIfFailed(
        (*newSubmix)->SetEffectParameters(0, parameters, sizeof(m_reverbParametersSmall))
    );

    if (enableEffect)
    {
        DX::ThrowIfFailed(
            (*newSubmix)->EnableEffect(0)
        );
    }

    DX::ThrowIfFailed(
        (*newSubmix)->SetVolume (1.0f)
    );

    float outputMatrix[4] = {0, 0, 0, 0};
    DX::ThrowIfFailed(
        (*newSubmix)->SetOutputMatrix(masteringVoice, 2, 2, outputMatrix)
    );
}

void Audio::CreateSourceVoice(SoundEvent sound)
{
    // Load all data for each sound effect into a single in-memory buffer
    MediaStreamer soundEffectStream;
    switch (sound)
    {
		case SoundEvent::CheckpointEvent:
			soundEffectStream.Initialize(L"Media\\Audio\\onsuccessfulsave.wav");
			break;
	
		case SoundEvent::MenuChangeEvent:
			soundEffectStream.Initialize(L"Media\\Audio\\onmenuselectionchange.wav"); 
			break;

        case SoundEvent::MenuSelectedEvent:
			soundEffectStream.Initialize(L"Media\\Audio\\onmenuitempressed.wav");
			break;

		case SoundEvent::MenuTiltEvent:
			soundEffectStream.Initialize(L"Media\\Audio\\onmenuselectionfrustra.wav"); 
			break;
	
		case SoundEvent::HitCrumble:
			soundEffectStream.Initialize(L"Media\\Audio\\decay-crumble.wav");
			break;

		case SoundEvent::ProjectileDecay:
			soundEffectStream.Initialize(L"Media\\Audio\\projectile-decay.wav");
			break;

		case SoundEvent::ClickSlap:
			soundEffectStream.Initialize(L"Media\\Audio\\click-slap.wav");
			break;

		case SoundEvent::ClickTikk:
			soundEffectStream.Initialize(L"Media\\Audio\\click-tikk.wav");
			break;

		case SoundEvent::Kaching:
			soundEffectStream.Initialize(L"Media\\Audio\\click-slap.wav");
			break;

		default:
			return;
	}

	this->m_soundEffects[sound].m_soundEventType = sound;
    uint32 bufferLength = soundEffectStream.GetMaxStreamLengthInBytes();
	this->m_soundEffects[sound].m_soundEffectBufferData = new byte[bufferLength];
    soundEffectStream.ReadAll(this->m_soundEffects[sound].m_soundEffectBufferData, bufferLength, &m_soundEffects[sound].m_soundEffectBufferLength);

    XAUDIO2_SEND_DESCRIPTOR descriptors[3];
    descriptors[0].pOutputVoice = this->m_soundEffectMasteringVoice;
    descriptors[0].Flags = 0;
    descriptors[1].pOutputVoice = this->m_soundEffectReverbVoiceSmallRoom;
    descriptors[1].Flags = 0;
    descriptors[2].pOutputVoice = this->m_soundEffectReverbVoiceLargeRoom;
    descriptors[2].Flags = 0;
    XAUDIO2_VOICE_SENDS sends = {0};
    sends.SendCount = 3;
    sends.pSends = descriptors;

    // The rolling sound can have pitch shifting and a low-pass filter.
    if (sound == RollingEvent)
    {
        DX::ThrowIfFailed(
			this->m_soundEffectEngine->CreateSourceVoice(
                &this->m_soundEffects[sound].m_soundEffectSourceVoice,
                &(soundEffectStream.GetOutputWaveFormatEx()),
                XAUDIO2_VOICE_USEFILTER,
                2.0f,
                &this->m_voiceContext,
                &sends)
            );
    }
    else
    {
        DX::ThrowIfFailed(
			this->m_soundEffectEngine->CreateSourceVoice(
                &this->m_soundEffects[sound].m_soundEffectSourceVoice,
                &(soundEffectStream.GetOutputWaveFormatEx()),
                0,
                1.0f,
                &this->m_voiceContext,
                &sends)
            );
    }

	this->m_soundEffects[sound].m_soundEffectSampleRate = soundEffectStream.GetOutputWaveFormatEx().nSamplesPerSec;

    // Queue in-memory buffer for playback
    ZeroMemory(&this->m_soundEffects[sound].m_audioBuffer, sizeof(this->m_soundEffects[sound].m_audioBuffer));
	this->m_soundEffects[sound].m_audioBuffer.AudioBytes = this->m_soundEffects[sound].m_soundEffectBufferLength;
	this->m_soundEffects[sound].m_audioBuffer.pAudioData = this->m_soundEffects[sound].m_soundEffectBufferData;
	this->m_soundEffects[sound].m_audioBuffer.pContext = &this->m_soundEffects[sound];
	this->m_soundEffects[sound].m_audioBuffer.Flags = XAUDIO2_END_OF_STREAM;
    if (sound == RollingEvent)
    {
		this->m_soundEffects[sound].m_audioBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
    }
    else
    {
		this->m_soundEffects[sound].m_audioBuffer.LoopCount = 0;
    }

    if (sound == FallingEvent || sound == CheckpointEvent)
    {
		this->m_soundEffects[sound].m_soundEffectSourceVoice->SetVolume(0.4f);
    }

    DX::ThrowIfFailed
	(
		this->m_soundEffects[sound].m_soundEffectSourceVoice->SubmitSourceBuffer(&this->m_soundEffects[sound].m_audioBuffer)
    );
}

void Audio::ReleaseResources()
{
    if (m_musicSourceVoice != nullptr)
    {
        m_musicSourceVoice->DestroyVoice();
    }

    if (m_soundEffectReverbVoiceSmallRoom != nullptr)
    {
        m_soundEffectReverbVoiceSmallRoom->DestroyVoice();
    }

    if (m_soundEffectReverbVoiceLargeRoom != nullptr)
    {
        m_soundEffectReverbVoiceLargeRoom->DestroyVoice();
    }

    if (m_musicReverbVoiceSmallRoom != nullptr)
    {
        m_musicReverbVoiceSmallRoom->DestroyVoice();
    }

    if (m_musicReverbVoiceLargeRoom != nullptr)
    {
        m_musicReverbVoiceLargeRoom->DestroyVoice();
    }

    for (int i = 0; i < ARRAYSIZE(m_soundEffects); i++)
    {
        if (m_soundEffects[i].m_soundEffectSourceVoice != nullptr)
        {
            m_soundEffects[i].m_soundEffectSourceVoice->DestroyVoice();
        }

        m_soundEffects[i].m_soundEffectSourceVoice = nullptr;
    }

    if (m_musicMasteringVoice != nullptr)
    {
            m_musicMasteringVoice->DestroyVoice();
    }

    if (m_soundEffectMasteringVoice != nullptr)
    {
        m_soundEffectMasteringVoice->DestroyVoice();
    }

	this->m_musicSourceVoice = nullptr;
	this->m_musicMasteringVoice = nullptr;
	this->m_soundEffectMasteringVoice = nullptr;
	this->m_soundEffectReverbVoiceSmallRoom = nullptr;
	this->m_soundEffectReverbVoiceLargeRoom = nullptr;
	this->m_musicReverbVoiceSmallRoom = nullptr;
	this->m_musicReverbVoiceLargeRoom = nullptr;
	this->SafeRelease(&m_musicEngine);
	this->SafeRelease(&m_soundEffectEngine);
}

void Audio::Start()
{
    if (m_engineExperiencedCriticalError)
    {
        return;
    }

    HRESULT hr = m_musicSourceVoice->Start(0);
    if SUCCEEDED(hr) 
	{
        m_isMusicStarted = true;
    }
    else
    {
        m_engineExperiencedCriticalError = true;
    }
}

// This sample processes audio buffers during the render cycle of the application.
// As long as the sample maintains a high-enough frame rate, this approach should
// not glitch audio. In game code, it is best for audio buffers to be processed
// on a separate thread that is not synced to the main render loop of the game.
void Audio::Render()
{
    if (m_engineExperiencedCriticalError)
    {
        this->m_engineExperiencedCriticalError = false;
        this->ReleaseResources();
        this->Initialize();
        this->CreateResources();
        this->Start();
        if (this->m_engineExperiencedCriticalError)
        {
            return;
        }
    }

    try
    {
        bool streamComplete;
        XAUDIO2_VOICE_STATE state;
        uint32 bufferLength;
        XAUDIO2_BUFFER buf = {0};

        // Use MediaStreamer to stream the buffers.
        this->m_musicSourceVoice->GetState(&state);
        while (state.BuffersQueued <= MAX_BUFFER_COUNT - 1)
        {
            streamComplete = m_musicStreamer.GetNextBuffer(
                m_audioBuffers[m_currentBuffer],
                STREAMING_BUFFER_SIZE,
                &bufferLength
                );

            if (bufferLength > 0)
            {
                buf.AudioBytes = bufferLength;
                buf.pAudioData = m_audioBuffers[m_currentBuffer];
                buf.Flags = (streamComplete) ? XAUDIO2_END_OF_STREAM : 0;
                buf.pContext = 0;
                DX::ThrowIfFailed(
                    m_musicSourceVoice->SubmitSourceBuffer(&buf)
                    );

                m_currentBuffer++;
                m_currentBuffer %= MAX_BUFFER_COUNT;
            }

            if (streamComplete)
            {
                // Loop the stream.
                m_musicStreamer.Restart();
                break;
            }

            m_musicSourceVoice->GetState(&state);
        }
    }
    catch (...)
    {
        m_engineExperiencedCriticalError = true;
    }
}

void Audio::PlaySoundEffect(SoundEvent sound)
{
    XAUDIO2_BUFFER buf = {0};
    XAUDIO2_VOICE_STATE state = {0};
    if (m_engineExperiencedCriticalError) 
	{
        /* if there's an error, then we'll recreate the engine on the next render pass */
        return;
    }

    SoundEffectData* soundEffect = &m_soundEffects[sound];
    HRESULT hr = soundEffect->m_soundEffectSourceVoice->Start();
    if FAILED(hr)
    {
        m_engineExperiencedCriticalError = true;
        return;
    }

    // For one-off voices, submit a new buffer if there's none queued up,
    // and allow up to two collisions to be queued up.
    if (sound != RollingEvent)
    {
        XAUDIO2_VOICE_STATE state = {0};
        soundEffect->m_soundEffectSourceVoice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
        if (state.BuffersQueued == 0)
        {
            soundEffect->m_soundEffectSourceVoice->SubmitSourceBuffer(&soundEffect->m_audioBuffer);
        }
        else if (state.BuffersQueued < 2 && sound == CollisionEvent)
        {
            soundEffect->m_soundEffectSourceVoice->SubmitSourceBuffer(&soundEffect->m_audioBuffer);
        }

        // For the menu clicks, we want to stop the voice and replay the click right away.
        // Note that stopping and then flushing could cause a glitch due to the
        // waveform not being at a zero-crossing, but due to the nature of the sound
        // (fast and 'clicky'), we don't mind.
		if (state.BuffersQueued > 0 && (sound == MenuChangeEvent || sound == MenuTiltEvent))
		{
			soundEffect->m_soundEffectSourceVoice->Stop();
			soundEffect->m_soundEffectSourceVoice->FlushSourceBuffers();
			soundEffect->m_soundEffectSourceVoice->SubmitSourceBuffer(&soundEffect->m_audioBuffer);
			soundEffect->m_soundEffectSourceVoice->Start();
		}
    }

    m_soundEffects[sound].m_soundEffectStarted = true;
}

void Audio::StopSoundEffect(SoundEvent sound)
{
    if (m_engineExperiencedCriticalError) {
        return;
    }

    HRESULT hr = m_soundEffects[sound].m_soundEffectSourceVoice->Stop();
    if FAILED(hr)
    {
        // If there's an error, then we'll recreate the engine on the next render pass
        m_engineExperiencedCriticalError = true;
        return;
    }

    m_soundEffects[sound].m_soundEffectStarted = false;
}

bool Audio::IsSoundEffectStarted(SoundEvent sound)
{
    return m_soundEffects[sound].m_soundEffectStarted;
}

void Audio::SetSoundEffectVolume(SoundEvent sound, float volume)
{
    if (m_soundEffects[sound].m_soundEffectSourceVoice != nullptr) {
        m_soundEffects[sound].m_soundEffectSourceVoice->SetVolume(volume);
    }
}

void Audio::SetSoundEffectPitch(SoundEvent sound, float pitch)
{
    if (m_soundEffects[sound].m_soundEffectSourceVoice != nullptr) {
        m_soundEffects[sound].m_soundEffectSourceVoice->SetFrequencyRatio(pitch);
    }
}

void Audio::SetSoundEffectFilter(SoundEvent sound, float frequency, float oneOverQ)
{
    if (m_soundEffects[sound].m_soundEffectSourceVoice != nullptr) {
        if (oneOverQ <= 0.1f)
        {
            oneOverQ = 0.1f;
        }
        if (oneOverQ > XAUDIO2_MAX_FILTER_ONEOVERQ)
        {
            oneOverQ = XAUDIO2_MAX_FILTER_ONEOVERQ;
        }
        XAUDIO2_FILTER_PARAMETERS params = {LowPassFilter, XAudio2CutoffFrequencyToRadians(frequency, m_soundEffects[sound].m_soundEffectSampleRate), oneOverQ};

        m_soundEffects[sound].m_soundEffectSourceVoice->SetFilterParameters(&params);
    }
}

bool Audio::IsMusicStarted()
{
	return this->m_isMusicStarted;
}

bool Audio::IsSfxStarted()
{
	return this->m_isSfxStarted;
}

bool Audio::IsMusicSuspended()
{
	return this->m_isMusicPaused;
}

bool Audio::IsSfxSuspended()
{
	return this->m_isSfxPaused;
}

bool Audio::IsMusicPlaying()
{
	return this->m_isMusicStarted && !this->m_isMusicPaused;
}

bool Audio::IsSfxPlaying()
{
	return this->m_isSfxStarted && !this->m_isSfxPaused;
}

// Uses the IXAudio2::StopEngine method to stop all audio immediately.
// It leaves the audio graph untouched, which preserves all effect parameters
// and effect histories (like reverb effects) voice states, pending buffers,
// cursor positions and so on.
// When the engines are restarted, the resulting audio will sound as if it had
// never been stopped except for the period of silence.
void Audio::SuspendMusic()
{
    if (this->m_engineExperiencedCriticalError)
    {
        return;
    }

    if (this->m_isMusicStarted)
    {
		this->m_musicEngine->StopEngine();
		this->m_isMusicPaused = true;
	}
}

void Audio::SuspendSfx()
{
	if (this->m_engineExperiencedCriticalError)
	{
		return;
	}

	if (this->m_isSfxStarted)
	{
		this->m_soundEffectEngine->StopEngine();
		this->m_isSfxPaused = true;
	}
}

// Restarts the audio streams. A call to this method must match a previous call
// to SuspendAudio. This method causes audio to continue where it left off.
// If there is a problem with the restart, the m_engineExperiencedCriticalError
// flag is set. The next call to Render will recreate all the resources and
// reset the audio pipeline.
void Audio::ResumeMusic()
{
    if (this->m_engineExperiencedCriticalError || !this->m_isMusicPaused)
    {
        return;
    }

    HRESULT hr = this->m_musicEngine->StartEngine();
    if (FAILED(hr))
    {
		this->m_engineExperiencedCriticalError = true;
    }
	else
	{
		this->m_isMusicPaused = false;
	}	
}

void Audio::ResumeSfx()
{
	if (this->m_engineExperiencedCriticalError || !this->m_isSfxPaused)
	{
		return;
	}

	HRESULT hr = this->m_soundEffectEngine->StartEngine();
	if (FAILED(hr))
	{
		this->m_engineExperiencedCriticalError = true;
	}
	else
	{
		this->m_isSfxPaused = false;
	}
}
