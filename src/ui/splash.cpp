#include "splash.h"

#include "menu-common.h"
#include "../../main.h"
#include "../../res/chunk-constants.h"
#include "../util/resutil.h"
#include "../shared-constants.h"

namespace blooDot::Splash {
    bool splashRunning = false;
    SDL_Texture* splashTexture = nullptr;
    double backgroundPosX = 0.0;
    double backgroundPosY = 0.0;
    constexpr int backdropHorz = 1280 - Constants::GodsPreferredWidth;
    constexpr int backdropVert = 720 - Constants::GodsPreferredHight;
    MenuCommon::MenuDialogInteraction menuState;

    bool LoadSplash(SDL_Renderer* renderer) {
        SDL_FRect splashRect;
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
            MenuCommon::HandleMenu(&menuState);

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