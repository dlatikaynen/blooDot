#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "brotli/c/common/version.h"
#include "xassy/enxassy.h"
#include "src/util/bytefmt.h"
#include "xassy/dexassy.h"

constexpr int RETVAL_OK = 0;
constexpr int RETVAL_SDL_INIT_FAIL = 0xacab01;

int xassy();
int main(const int argc, char *argv[]) {
    const auto NotG = "blooDot";
    const auto version = "4.0.0";
    int retVal = RETVAL_OK;

    std::cout << "Hello there and welcome to " << NotG << " " << version << "!\n";
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
        const auto initError = SDL_GetError();

        std::cerr << "Failed to initialize video subsystem, KTHXBYE: " << initError << std::endl;

        return RETVAL_SDL_INIT_FAIL;
    }

    const auto sdlVersion = SDL_GetVersion();
    const auto sdlVersionMajor = SDL_VERSIONNUM_MAJOR(sdlVersion);
    const auto sdlVersionMinor = SDL_VERSIONNUM_MINOR(sdlVersion);
    const auto sdlVersionMicro = SDL_VERSIONNUM_MICRO(sdlVersion);

    std::cout << "SDL version: " << sdlVersionMajor << "." << sdlVersionMinor << "." << sdlVersionMicro << std::endl;
    std::cout << "TTF version: " << SDL_TTF_MAJOR_VERSION << "." << SDL_TTF_MINOR_VERSION << "." << SDL_TTF_MICRO_VERSION << std::endl;
    std::cout << "IMG version: " << SDL_IMAGE_MAJOR_VERSION << "." << SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_MICRO_VERSION << std::endl;
    std::cout << "Brotli version: " << BROTLI_VERSION_MAJOR << "." << BROTLI_VERSION_MINOR << "." << BROTLI_VERSION_PATCH << std::endl;

    auto cookedOpen = false;
    for (auto i = 0; i < 1; ++i) {
        if (argc > 1 && strnicmp(argv[1], "xassy", 0xff) == 0) {
            retVal = xassy();

            break;
        }

        std::cout << "AHALLO.\n";

        /* https://stackoverflow.com/a/31926842/1132334 */
        ::PrepareIndex();
        cookedOpen = OpenCooked();
        if (!cookedOpen) {
            break;
        }
    }

    if (cookedOpen) {
        CloseCooked();
    }

    SDL_Quit();

    return retVal;
}

void ReportError(const char* message, const char* error)
{
    std::cerr
        << message
        << ", whatever that means, "
        << error
        << "\n";
}

int xassy() {
    XassyCookInfo cookStats{};
    Cook(&cookStats);

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

    return RETVAL_OK;
}