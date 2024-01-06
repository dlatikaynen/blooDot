#include "pch.h"
#include "music.h"
#include <random>

namespace blooDot::Music
{
	bool isProcessing = false;
	std::thread processingThread;
	SDL_AudioDeviceID speaker = 0;
	SDL_AudioSpec spec;
	AudioFrame currentFrame;
	std::random_device seed;
	std::mt19937 gen{ seed() };
	std::uniform_int_distribution<> dist{ 40, 88 };
	std::uniform_real_distribution<> noise{ -1., 1. };

	bool StartMusic()
	{
		SDL_AudioSpec desiro;
		SDL_zero(desiro);
		desiro.freq = Frequency;
		desiro.channels = SampleChannels;
		desiro.samples = FrameSize / desiro.channels;
		desiro.format = AUDIO_S32;
		speaker = SDL_OpenAudioDevice(nullptr, 0, &desiro, &spec, 0);
		if (speaker == 0)
		{
			const auto startError = SDL_GetError();

			ReportError("Could not start playing music", startError);

			return false;
		}

		SDL_PauseAudioDevice(speaker, 0);
		isProcessing = true;
		processingThread = std::thread(_generator);

		return true;
	}

	void _generator()
	{
		Oscillator::Oscillator oscillator;
		Oscillator::StringShape instrument;
		oscillator.AddSynthesizer(&instrument);

		while (isProcessing)
		{
			const auto note = static_cast<double>(dist(gen));
			oscillator.Start(note, 100.0);
			for (size_t i = 0; i < currentFrame.size(); ++i)
			{
				const auto frame = oscillator.Render();
				
				currentFrame[i] = static_cast<int>(frame * INT_MAX); //dist(gen);
			}

			instrument.Clear();
			oscillator.CutOut();

			constexpr size_t twiceBuf = currentFrame.size() * sizeof(int) * 2;
			constexpr auto frameSize = static_cast<Uint32>(currentFrame.size() * sizeof(int));

			while (SDL_GetQueuedAudioSize(speaker) >= twiceBuf)
			{
				/* we enqueue no more than two buffers full of data,
				 * so the reaction time for abrupt controlled music
				 * change stays conveniently small */
				SDL_Delay(10);
			}

			if (SDL_QueueAudio(speaker, currentFrame.data(), frameSize) != 0)
			{
				const auto queueError = SDL_GetError();
				
				ReportError("Could not enqueue more music", queueError);
			}
		}
	}

	void Teardown()
	{
		isProcessing = false;
		processingThread.join();
		if (speaker > 0)
		{
			SDL_PauseAudioDevice(speaker, 1);
			SDL_CloseAudioDevice(speaker);
		}

		speaker = 0;
	}
}
