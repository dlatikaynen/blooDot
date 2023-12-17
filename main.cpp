#include "pch.h"
#include "main.h"
#include "dexassy.h"
#include "scripture.h"
#include "splash.h"
#include "orchestrator.h"
#include "huff-comp.h"
#include "huff-deco.h"
#include "constants.h"
#include "sfx.h"
#include "xlations.h"
#include "settings.h"
#include "playerstate.h"
#include "controller-stuff.h"

#ifndef NDEBUG
#include "enxassy.h"
#include "enxlate.h"
#endif
#include "shader-engine.h"
using namespace blooDot::ShaderEngine;

const int major = 3;
const int minor = 1;
const int patch = 0;

const int ExitCodeNormally = 0x00;
const int ExitCodeSDLInitFail = 0x55;
const size_t MaxExpectedSDLErrorLength = 0x8000;
const char* NameOfTheGame = "blooDot";

extern SettingsStruct Settings;

SDL_Window* mainWindow = NULL;
SDL_Renderer* renderer = NULL;

void ReportError(const char* message, const char* error)
{
	std::cerr
		<< message
		<< ", whatever that means, "
		<< error
		<< "\n";
}

#ifndef NDEBUG
extern int Cook(XassyCookInfo*);
extern int ::Xlate(XassyXlatInfo *info);

int main(int argc, char** argv)
{
	if (argc == 2 && _strnicmp(argv[1], "xassy", _MAX_PATH) == 0)
	{
		XassyCookInfo cookStats;
		XassyXlatInfo xlatStats;
		auto const& cooked = Cook(&cookStats);
		auto const& xlated = Xlate(&xlatStats);
		std::cout 
			<< "Xassy has assembled "
			<< cookStats.numFiles
			<< " items, getting down to "
			<< cookStats.numBytesAfter
			<< " from originally "
			<< cookStats.numBytesBefore
			<< " bytes\n"
			<< "Xassy has assembled "
			<< xlatStats.numLiterals
			<< " literals from "
			<< xlatStats.numFiles
			<< " file(s)\n"
			<< "cooked "
			<< cooked
			<< ", xlated " 
			<< xlated 
			<< ((cooked || xlated) ? " \033[31mFAIL" : " \033[32mSUCCESS")
			<< "\033[0m"
			<< '\n';
		system("pause");
		return cooked || xlated ? 0xacab : 0;
	}

#else
int main(int, char**)
{
#endif
	/* startup */
	std::cout
		<< NameOfTheGame 
		<< " Version "
		<< unsigned(major)
		<< "."
		<< unsigned(minor)
		<< "."
		<< unsigned(patch)
		<< "\n";

	const auto initResult = SDL_Init(SDL_INIT_EVERYTHING | SDL_VIDEO_OPENGL);
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

	std::cout << "Current video driver is " << SDL_GetCurrentVideoDriver() << "\n";
	while (true)
	{
		/* action */
		mainWindow = SDL_CreateWindow(
			NameOfTheGame,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			GodsPreferredWidth,
			GodsPreferredHight,
			0
			| SDL_WINDOW_HIDDEN
			| SDL_WINDOW_ALLOW_HIGHDPI
			| SDL_WINDOW_OPENGL
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

		renderer = SDL_CreateRenderer(mainWindow, -2, SDL_RENDERER_ACCELERATED);
		if (!renderer)
		{
			const auto rendererError = SDL_GetError();
			ReportError("Failed to create a GPU renderer", rendererError);
			break;
		}

		SDL_RendererInfo rendererInfo;
		if(SDL_GetRendererInfo(renderer, &rendererInfo) != 0)
		{
			const auto infoError = SDL_GetError();
			ReportError("Failed to obtain GPU renderer info", infoError);
			break;
		}

		std::cout << "Gfx engine is " << rendererInfo.name << std::endl;

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

		/* https://stackoverflow.com/a/31926842/1132334 */
		::PrepareIndex();
		if (!OpenCooked())
		{
			/* this happens when the assets file is not present */
			goto Abort;
		}

		blooDot::Sfx::PreloadMenuSfx();
		blooDot::Settings::PreloadControllerMappings();

		SDL_ShowWindow(mainWindow);
		blooDot::Sfx::Play(SoundEffect::SFX_BULLET_DECAY);
		if (blooDot::Splash::SplashLoop(renderer))
		{
			/* reaching this point means we're actually starting
			 * in the arena. we create a new window and renderer
			 * for the arena, even if the video mode matches */
			blooDot::Player::NumPlayers = 4;
			::_Launch();
		}

		blooDot::Sfx::Teardown();
		::Mix_Quit();
		::IMG_Quit();

		if (renderer)
		{
			SDL_DestroyRenderer(renderer);
		}

		if (mainWindow)
		{
			SDL_DestroyWindow(mainWindow);
		}

		break;
	}

	/* shitdown */
	blooDot::ControllerStuff::Teardown();
	::CloseCooked();
	::CloseFonts();
	::TeardownDialogControls();
Abort:
	SDL_ClearError();
	::SDL_Quit();
	const auto shutdownError = SDL_GetError();
	if (strnlen(shutdownError, MaxExpectedSDLErrorLength) > 0) {
		ReportError("Could not quit cleanly", shutdownError);
	}

	return ExitCodeNormally;
}

void _Launch()
{
	SDL_HideWindow(mainWindow);
	SDL_DestroyRenderer(renderer);
	renderer = NULL;

	SDL_DestroyWindow(mainWindow);
	mainWindow = NULL;

	switch(Settings.SettingViewportResolution)
	{
	case ViewportResolutions::VR_HERCMONO:
		mainWindow = SDL_CreateWindow(
			NameOfTheGame,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			720,
			348,
			0
			| SDL_WINDOW_HIDDEN
			| SDL_WINDOW_ALLOW_HIGHDPI
			| SDL_WINDOW_OPENGL
		);

		break;

	case ViewportResolutions::VR_MODEX:
		mainWindow = SDL_CreateWindow(
			NameOfTheGame,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			320,
			240,
			0
			| SDL_WINDOW_HIDDEN
			| SDL_WINDOW_ALLOW_HIGHDPI
			| SDL_WINDOW_OPENGL
		);

		break;

	case ViewportResolutions::VR_SVGA:
		mainWindow = SDL_CreateWindow(
			NameOfTheGame,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			800,
			600,
			0		
			| SDL_WINDOW_HIDDEN
			| SDL_WINDOW_ALLOW_HIGHDPI
			| SDL_WINDOW_OPENGL
		);

		break;

	case ViewportResolutions::VR_NOTEBOOK:
		mainWindow = SDL_CreateWindow(
			NameOfTheGame,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			1280,
			768,
			0
			| SDL_WINDOW_HIDDEN 
			| SDL_WINDOW_ALLOW_HIGHDPI
			| SDL_WINDOW_OPENGL
		);

		break;

	case ViewportResolutions::VR_SQUARE:
	case ViewportResolutions::VR_MAXOUT:
	{
		/* these are fullscreen modes. we remember the choice of monitor
		 * so we don't annoy users who have multiple displays (as one does) */
		const auto& numDisplays = SDL_GetNumVideoDisplays();
		int displayIndex = 0;
		if (numDisplays > 1)
		{
			const auto& configuredDisplayIndex = Settings.FullscreenDisplayIndex;
			if (configuredDisplayIndex >= 0 && configuredDisplayIndex < numDisplays)
			{
				displayIndex = configuredDisplayIndex;
			}
		}

		mainWindow = SDL_CreateWindow(
			NameOfTheGame,
			SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex),
			SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex),
			0,
			0,
			0
			| SDL_WINDOW_HIDDEN
			| SDL_WINDOW_OPENGL
			| SDL_WINDOW_BORDERLESS
			| SDL_WINDOW_ALLOW_HIGHDPI
			| SDL_WINDOW_FULLSCREEN_DESKTOP
		);

		break;
	}

	default:
		mainWindow = SDL_CreateWindow(
			NameOfTheGame,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			GodsPreferredWidth,
			GodsPreferredHight,
			0
			| SDL_WINDOW_HIDDEN
			| SDL_WINDOW_OPENGL
			| SDL_WINDOW_ALLOW_HIGHDPI
		);

		break;
	}

	if (!mainWindow)
	{
		const auto windowError = SDL_GetError();
		ReportError("Failed to create arena window", windowError);
		return;
	}

	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
	renderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (!renderer)
	{
		const auto rendererError = SDL_GetError();
		ReportError("Failed to create arena GPU renderer", rendererError);
		return;
	}

	SDL_RendererInfo rendererInfo;
	if (SDL_GetRendererInfo(renderer, &rendererInfo) != 0)
	{
		const auto infoError = SDL_GetError();
		ReportError("Failed to obtain renderer info", infoError);
		return;
	}

	std::cout
		<< "Using renderer "
		<< rendererInfo.name
		<< ", maxing out at "
		<< rendererInfo.max_texture_width
		<< " x "
		<< rendererInfo.max_texture_height
		<< "\n";

	if (rendererInfo.flags & SDL_RendererFlags::SDL_RENDERER_ACCELERATED)
	{
		std::cout << "This renderer is using the GPU\n";
	}

	if (rendererInfo.flags & SDL_RendererFlags::SDL_RENDERER_SOFTWARE)
	{
		std::cout << "This renderer supports software rendering\n";
	}

	if (rendererInfo.flags & SDL_RendererFlags::SDL_RENDERER_PRESENTVSYNC)
	{
		std::cout << "This renderer supports vertical flyback synchronization\n";
	}

	if (rendererInfo.flags & SDL_RendererFlags::SDL_RENDERER_TARGETTEXTURE)
	{
		std::cout << "This renderer supports rendering to textures\n";
	}

    if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) != 0)
	{
		const auto modeError = SDL_GetError();
		ReportError("Failed to set blend mode", modeError);
		return;
	}

	if (!InitGLExtensions())
	{
		std::cout << "Could not initialize OpenGL extensions" << std::endl;
		return;
	}

	SDL_ShowWindow(mainWindow);
	blooDot::Orchestrator::MainLoop(renderer, mainWindow);
}
