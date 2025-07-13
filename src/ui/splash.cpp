#include "splash.h"
#include "../../res/chunk-constants.h"
#include "../util/resutil.h"

namespace blooDot::Splash {
    bool splashRunning = false;
    SDL_Texture* splashTexture = nullptr;
    double backgroundPosX = 0.0;
    double backgroundPosY = 0.0;
    const int backdropHorz = 1280 - 640;
    const int backdropVert = 720 - 480;

    bool LoadSplash(SDL_Renderer* renderer) {
        SDL_Rect splashRect;
        splashTexture = Res::LoadPicture(renderer, CHUNK_KEY_SPLASH, &splashRect);
        if (!splashTexture)
        {
            return false;
        }

        splashRunning = true;

        return true;
    }

    bool SplashLoop(SDL_Renderer* renderer) {
        constexpr SDL_FRect srcRect{ 0,0,640,480 };
        constexpr SDL_FRect dstRect{ 0,0,640,480 };

        if (!LoadSplash(renderer)) {
            return false;
        }

        while (splashRunning) {
            if (!SDL_RenderClear(renderer))
            {
                const auto clearError = SDL_GetError();

                ReportError("Failed to clear the title screen", clearError);
            }

            if (!SDL_RenderTexture(renderer, splashTexture, &srcRect, &dstRect))
            {
                const auto blitError = SDL_GetError();

                ReportError("Failed to blit splash background", blitError);
                splashRunning = false;

                break;
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

        return true;
    }
}