﻿#include "pch.h"
#include "main.h"
#include "dexassy.h"
#include "scripture.h"
#include "splash.h"
#include "orchestrator.h"

const int ExitCodeNormally = 0x00;
const int ExitCodeSDLInitFail = 0x55;
const size_t MaxExpectedSDLErrorLength = 0x8000;
const int GodsFavoriteScreenWidth = 0x280;
const int GodsFavoriteScreenHeight = 0x1e0;
const char* NameOfTheGame = "blooDot";

void ReportError(const char* message, const char* error)
{
	std::cerr << message << ", whatever that means, " << error;
}

int main(int, char**)
{
	/* startup */
	const auto initResult = SDL_Init(SDL_INIT_EVERYTHING);
	if (initResult < 0)
	{
		const auto errorMessage = SDL_GetError();
		ReportError("Could not start SDL", errorMessage);
		return ExitCodeSDLInitFail;
	}

	SDL_version sdlVersion;
	SDL_GetVersion(&sdlVersion);
	std::cout 
		<< "Simple DirectMedia Library Version "
		<< unsigned(sdlVersion.major)
		<< "."
		<< unsigned(sdlVersion.minor)
		<< "."
		<< unsigned(sdlVersion.patch)
		<< "\n";

	const auto availableMb = SDL_GetSystemRAM();
	std::cout << "SDL reports " << availableMb << " MB of memory available\n";

	const auto runningOn = SDL_GetPlatform();
	std::cout << "Running on " << runningOn << "\n";
	std::cout << "Number of CPUs available is " << SDL_GetCPUCount() << ", with " << SDL_GetCPUCacheLineSize() << " cache lines\n";
	const auto numVideoDrivers = SDL_GetNumVideoDrivers();
	for (int i = 0; i < numVideoDrivers; ++i)
	{
		std::cout << "Video driver \"" << SDL_GetVideoDriver(i) << "\" present\n";
	}

	const auto connectedControllers = SDL_NumJoysticks();
	if (connectedControllers == 0)
	{
		std::cout << "No controllers detected\n";
	}
	else
	{
		for (int i = 0; i < connectedControllers; ++i)
		{
			const auto controllerName = SDL_GameControllerNameForIndex(i);
			if(controllerName) 
			{
				std::cout << "Controller \"" << controllerName << "\" present\n";
			}
			else
			{
				std::wcout << "Controller (no name) present\n";
			}
		}
	}

	while (true)
	{
		/* action */
		const auto mainWindow = SDL_CreateWindow(
			NameOfTheGame,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			GodsFavoriteScreenWidth,
			GodsFavoriteScreenHeight,
			SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL
		);

		if (!mainWindow)
		{
			const auto windowError = SDL_GetError();
			ReportError("Failed to create a window", windowError);
			break;
		}

		std::cout << "Created main window " << unsigned(SDL_GetWindowID(mainWindow)) << " with a resolution that pleases the Lord\n";
		if (IMG_Init(IMG_INIT_PNG) < 0)
		{
			const auto imageError = IMG_GetError();
			ReportError("Failed to initialize imaging", imageError);
			break;
		}

		const auto imagingVersion = IMG_Linked_Version();
		std::cout
			<< "Imaging Library Version "
			<< unsigned(imagingVersion->major)
			<< "."
			<< unsigned(imagingVersion->minor)
			<< "."
			<< unsigned(imagingVersion->patch)
			<< "\n";

		const auto renderer = SDL_CreateRenderer(mainWindow, -2, SDL_RENDERER_ACCELERATED);
		if (!renderer)
		{
			const auto rendererError = SDL_GetError();
			ReportError("Failed to create a GPU renderer", rendererError);
			break;
		}

		if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 8192) < 0)
		{
			const auto mixerError = Mix_GetError();
			ReportError("Failed to initialize audio", mixerError);
			break;
		}

		const auto numAudioDrivers = SDL_GetNumAudioDrivers();
		for (auto i = 0; i < numAudioDrivers; ++i)
		{
			std::cout << "Audio driver \"" << SDL_GetAudioDriver(i) << "\" present\n";
		}

		const auto mixerVersion = Mix_Linked_Version();
		std::cout
			<< "Initialized audio mixer version "
			<< unsigned(mixerVersion->major)
			<< "."
			<< unsigned(mixerVersion->minor)
			<< "."
			<< unsigned(mixerVersion->patch)
			<< "\n";

		std::cout << "AHALLO.\n";

		PrepareIndex();
		OpenCooked();

		SDL_RWops* soundFile;
		const auto soundMem = Retrieve(CHUNK_KEY_SFX_PROJECTILEDECAY, &soundFile);
		const auto soundEffect = Mix_LoadWAV_RW(soundFile, true);
		soundFile->close(soundFile);
		SDL_free(soundMem);
		if (!soundEffect)
		{
			const auto soundError = SDL_GetError();
			ReportError("Failed to load sound effect", soundError);
			break;
		}

		SDL_ShowWindow(mainWindow);
		Mix_PlayChannel(-1, soundEffect, 0);
		SplashLoop(renderer);
		MainLoop(renderer);
		Mix_FreeChunk(soundEffect);
		Mix_Quit();
		IMG_Quit();
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(mainWindow);

		break;
	}

	/* shitdown */
	CloseCooked();
	CloseFonts();
	SDL_ClearError();
	SDL_Quit();
	const auto shutdownError = SDL_GetError();
	if (strnlen(shutdownError, MaxExpectedSDLErrorLength) > 0) {
		ReportError("Could not quit cleanly", shutdownError);
	}

	return ExitCodeNormally;
}
