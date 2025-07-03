#include <iostream>
#include <SDL3/SDL.h>

#include "brotli/c/common/version.h"
#include "xassy/enxassy.h"

int main() {
    const auto lang = "C++";

    std::cout << "Hello and welcome to " << lang << "!\n";

    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        const auto initError = SDL_GetError();

        std::cout << "Failed to initialize video subsystem: " << initError << std::endl;
    }

    const auto sdlVersion = SDL_GetVersion();
    const auto sdlVersionMajor = SDL_VERSIONNUM_MAJOR(sdlVersion);
    const auto sdlVersionMinor = SDL_VERSIONNUM_MINOR(sdlVersion);
    const auto sdlVersionMicro = SDL_VERSIONNUM_MICRO(sdlVersion);

    std::cout << "SDL version: " << sdlVersionMajor << "." << sdlVersionMinor << "." << sdlVersionMicro << std::endl;
    std::cout << "Brotli version: " << BROTLI_VERSION_MAJOR << "." << BROTLI_VERSION_MINOR << "." << BROTLI_VERSION_PATCH << std::endl;

    XassyCookInfo cookStats{};
    Cook(&cookStats);

    return 0;
}