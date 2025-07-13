#include "main.h"

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "brotli/c/common/version.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "src/shared-constants.h"
#include "src/snd/sfx.h"
#include "src/ui/splash.h"
#include "xassy/enxassy.h"
#include "src/util/bytefmt.h"
#include "xassy/dexassy.h"
#include "xassy/enxlate.h"

int main(const int argc, char *argv[]) {
    const auto version = "4.0.0";
    int retVal = blooDot::RETVAL_OK;

    std::cout << "Hello there and welcome to " << blooDot::NotG << " " << version << "!\n";
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
        const auto initError = SDL_GetError();

        std::cerr << "Failed to initialize video subsystem, KTHXBYE: " << initError << std::endl;

        return blooDot::RETVAL_SDL_INIT_FAIL;
    }

    const auto sdlVersion = SDL_GetVersion();
    const auto sdlVersionMajor = SDL_VERSIONNUM_MAJOR(sdlVersion);
    const auto sdlVersionMinor = SDL_VERSIONNUM_MINOR(sdlVersion);
    const auto sdlVersionMicro = SDL_VERSIONNUM_MICRO(sdlVersion);

    std::cout << "SDL version: " << sdlVersionMajor << "." << sdlVersionMinor << "." << sdlVersionMicro << std::endl;
    std::cout << "TTF version: " << SDL_TTF_MAJOR_VERSION << "." << SDL_TTF_MINOR_VERSION << "." << SDL_TTF_MICRO_VERSION << std::endl;
    std::cout << "IMG version: " << SDL_IMAGE_MAJOR_VERSION << "." << SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_MICRO_VERSION << std::endl;
    std::cout << "Brotli version: " << BROTLI_VERSION_MAJOR << "." << BROTLI_VERSION_MINOR << "." << BROTLI_VERSION_PATCH << std::endl;

    const size_t availableRam = static_cast<size_t>(SDL_GetSystemRAM()) << 20;
    const auto& runningOn = SDL_GetPlatform();
    const auto& numVideoDrivers = SDL_GetNumVideoDrivers();
    const auto& currentVideoDriver = SDL_GetCurrentVideoDriver();
    const auto& imagingVersion = IMG_Version();

    std::cout << "SDL reports " << blooDot::bytesize(availableRam) << " available\n";
    std::cout << "Running on " << runningOn << "\n";
    std::cout << "Number of logical CPU cores available is " << SDL_GetNumLogicalCPUCores() << ", with " << SDL_GetCPUCacheLineSize() << " cache lines\n";
    for (int i = 0; i < numVideoDrivers; ++i)
    {
        if (const auto& driverName = SDL_GetVideoDriver(i); strnicmp(driverName, "dummy", 0xff) == 0) {
            if (strnicmp(currentVideoDriver, "dummy", 0xff) != 0) {
                continue;
            }
        }

        std::cout << "Video driver \"" << SDL_GetVideoDriver(i) << "\" present\n";
    }

    std::cout << "Current video driver is " << currentVideoDriver << "\n";

    const auto& numRenderDrivers = SDL_GetNumRenderDrivers();

    if (numRenderDrivers == 0) {
        std::cerr << "No render drivers available";

        return blooDot::RETVAL_SDL_NO_RENDER_DRIVERS_FAIL;
    }

    for (auto i = 0; i < numRenderDrivers; ++i) {
        std::cout << "Available video render driver #" << i + 1 << ": " << SDL_GetRenderDriver(i) << "\n";
    }

    std::cout
        << "Imaging Library Version "
        << SDL_VERSIONNUM_MAJOR(imagingVersion)
        << "."
        << SDL_VERSIONNUM_MINOR(imagingVersion)
        << "."
        << SDL_VERSIONNUM_MICRO(imagingVersion)
        << "\n";

    std::cout
        << "Audio Library Version "
        << SDL_MIXER_MAJOR_VERSION
        << "."
        << SDL_MIXER_MINOR_VERSION
        << "."
        << SDL_MIXER_MICRO_VERSION
        << "\n";

    const auto numAudioDrivers = SDL_GetNumAudioDrivers();
    for (auto i = 0; i < numAudioDrivers; ++i)
    {
        const auto& audioDriver = SDL_GetAudioDriver(i);

        if (strnicmp(audioDriver, "dummy", 0xff) != 0) {
            std::cout << "Audio driver \"" << audioDriver << "\" present\n";
        }
    }

    SDL_AudioSpec spec;
    spec.channels = 8;
    spec.format = SDL_AUDIO_UNKNOWN;
    spec.freq = 22050;

    if (!Mix_OpenAudio(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec))
    {
        const auto mixerError = SDL_GetError();

        blooDot::ReportError("FAIL Could not initialize audio", mixerError);

        return blooDot::RETVAL_AUDIO_INIT_FAIL;
    }

    auto cookedOpen = false;
    for (auto i = 0; i < 1; ++i) {
        if (argc > 1 && strnicmp(argv[1], "xassy", 0xff) == 0) {
            retVal = blooDot::xassy();

            break;
        }

        std::cout << "AHALLO.\n";
        std::flush(std::cout);

        /* https://stackoverflow.com/a/31926842/1132334 */
        ::PrepareIndex();
        cookedOpen = OpenCooked();
        if (!cookedOpen) {
            retVal = blooDot::RETVAL_DEXASSY_FAIL;

            break;
        }

        blooDot::Sfx::PreloadMenuSfx();
        //blooDot::Settings::PreloadControllerMappings();
        //const auto musicStarted = blooDot::Music::StartMusic();

        SDL_ShowWindow(blooDot::mainWindow);
        blooDot::Sfx::Play(blooDot::Sfx::SoundEffect::SFX_BULLET_DECAY);

        while (!blooDot::Quit) {
            // one game session oscillates back and forth between
            // the outer UI and the arena. the arena can only be reached
            // again via the outer UI (the "main menu"). the arena can
            // quit out to the operating system directly though
            if (!blooDot::CreateMainUiWindow()) {
                retVal = blooDot::RETVAL_CREATE_MAIN_UI_FAIL;

                break;
            }

            // process the "main menu" and clean it up
            if (blooDot::Splash::SplashLoop(blooDot::renderer)) {
                // noop
            }

            blooDot::CleanupMainUiWindow();

            // if the outcome of the main menu is to start a game,
            // initialize and open the arena. if the outcome of the main menu
            // is to quit, we quit now
            if (blooDot::Quit) {
                break;
            }

            // process the arena with whatever launch configuration we got
            SDL_Delay(16);
        }
    }

    if (cookedOpen) {
        CloseCooked();
    }

    if (blooDot::renderer) {
        SDL_DestroyRenderer(blooDot::renderer);
    }

    if (blooDot::mainWindow) {
        SDL_DestroyWindow(blooDot::mainWindow);
    }

    blooDot::Sfx::Teardown();
    Mix_Quit();
    SDL_ClearError();
    SDL_Quit();

    if (const auto shutdownError = SDL_GetError(); strnlen(shutdownError, 1) > 0) {
        blooDot::ReportError("WARN Could not quit cleanly", shutdownError);

        if (retVal == blooDot::RETVAL_OK) {
            retVal = blooDot::RETVAL_NO_CLEAN_SHUTDOWN;
        }
    }

    return retVal;
}

namespace blooDot {
    void ReportError(const char* message, const char* error)
    {
        std::cerr
            << message
            << ", whatever that means, "
            << error
            << "\n"
            << std::flush;
    }

    int xassy() {
        XassyCookInfo cookStats{};

        if (const auto cookResult = Cook(&cookStats); cookResult == CookReturnCodeSuccess) {
            const auto sizeRaw = blooDot::bytesize(cookStats.numBytesBefore);
            const auto sizeCooked = blooDot::bytesize(cookStats.numBytesAfter);

            std::cout
                << "Assembled "
                << cookStats.numFiles
                << " chunks from "
                << sizeRaw
                << " down to "
                << sizeCooked
                << "\n";
        } else {
            return cookResult;
        }

        XassyXlatInfo xlatStats{};
        if (const auto xlateResult = Xlate(&xlatStats); xlateResult == XlateReturnCodeSuccess) {
            std::cout
                << "Assembled "
                << xlatStats.numLiterals
                << " literals from "
                << xlatStats.numFiles
                << " file(s)\n";
        } else {
            return xlateResult;
        }

        return RETVAL_OK;
    }

    bool CreateMainUiWindow() {
        if (mainWindow) {
            ReportError("FAIL There is already a main window. Clean up the previous one, before you create another", "Window Manager");

            return false;
        }

        if (renderer) {
            ReportError("FAIL There is already a UI renderer. Clean up the previous one, before you create another", "Window Manager");
            return false;
        }

        mainWindow = SDL_CreateWindow(
            NotG,
            Constants::GodsPreferredWidth,
            Constants::GodsPreferredHight,
            0
            | SDL_WINDOW_HIDDEN
            | SDL_WINDOW_HIGH_PIXEL_DENSITY
            | SDL_WINDOW_OPENGL
        );

        if (!mainWindow)
        {
            const auto windowError = SDL_GetError();

            ReportError("FAIL Failed to create the main UI window", windowError);

            return false;
        }

        const auto& windowId = SDL_GetWindowID(mainWindow);

        if (windowId == 0) {
            const auto winIdError = SDL_GetError();

            ReportError("FAIL Failed to obtain window Id of main UI window", winIdError);

            return false;
        }

        std::cout << "Created main UI window #" << windowId << " with a resolution that pleases the Lord\n";

        if (!SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl")) {
            const auto hintError = SDL_GetError();

            ReportError("FAIL Failed to set GPU renderer hint", hintError);

            return false;
        }

        renderer = SDL_CreateRenderer(mainWindow, nullptr);
        if (!renderer)
        {
            const auto rendererError = SDL_GetError();

            ReportError("FAIL Failed to create a GPU renderer", rendererError);

            return false;
        }

        const auto& rendererProperties = SDL_GetRendererProperties(renderer);

        if(rendererProperties == 0) {
            const auto infoError = SDL_GetError();

            ReportError("FAIL Failed to obtain renderer info", infoError);

            return false;
        }

        std::cout
            << "Renderer of window #"
            << windowId
            << " is "
            << SDL_GetStringProperty(rendererProperties, SDL_PROP_RENDERER_NAME_STRING, "(none)")
            << "\n"
            << std::flush;


        if (!SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND)) {
            const auto modeError = SDL_GetError();

            ReportError("FAIL Failed to set blend mode", modeError);

            return false;
        }

        if (!SDL_SetWindowPosition(mainWindow,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED)) {
            const auto posError = SDL_GetError();

            ReportError("WARN Failed to set the position of the main UI window", posError);

            // this is not fatal; we do not exit
        }

        if (!SDL_ShowWindow(mainWindow)) {
            const auto showError = SDL_GetError();

            ReportError("FAIL Failed to bring up the main UI window", showError);

            return false;
        }

        if (!SDL_SyncWindow(mainWindow)) {
            const auto syncError = SDL_GetError();

            ReportError("WARN Failed to await the synchronization of the main UI window", syncError);

            // this is not fatal; we do not exit
        }

        return true;
    }

    void CleanupMainUiWindow() {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
        SDL_DestroyWindow(mainWindow);
        mainWindow = nullptr;
    }
}