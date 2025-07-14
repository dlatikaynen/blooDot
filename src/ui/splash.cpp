#include "splash.h"

#include "dialog-controls.h"
#include "drawing.h"
#include "menu-common.h"
#include "scripture.h"
#include "../../main.h"
#include "../../res/chunk-constants.h"
#include "../../res/xlations.h"
#include "../util/resutil.h"
#include "../shared-constants.h"
#include "../../src/state/settings.h"

namespace blooDot::Splash {
    SDL_Texture* splashTexture = nullptr;
    double backgroundPosX = 0.0;
    double backgroundPosY = 0.0;
    constexpr int backdropHorz = 1280 - Constants::GodsPreferredWidth;
    constexpr int backdropVert = 720 - Constants::GodsPreferredHight;
    MenuCommon::MenuDialogInteraction menuState{};

    SDL_FRect continueRect{ 0,0,0,0 };
    SDL_FRect loadRect{ 0,0,0,0 };
    SDL_FRect singleRect{ 0,0,0,0 };
    SDL_FRect localMultiRect{ 0,0,0,0 };
    SDL_FRect creatorModeRect{ 0,0,0,0 };
    SDL_FRect settingsRect{ 0,0,0,0 };
    SDL_FRect quitRect{ 0,0,0,0 };

    SDL_Texture* continueTexture = nullptr;
    SDL_Texture* loadTexture = nullptr;
    SDL_Texture* singleTexture = nullptr;
    SDL_Texture* localMultiTexture = nullptr;
    SDL_Texture* creatorModeTexture = nullptr;
    SDL_Texture* settingsTexture = nullptr;
    SDL_Texture* quitTexture = nullptr;

    MenuCommon::MenuDialogInteraction SplashLoop(SDL_Renderer* renderer) {
        menuState.leaveDialog = false;
        menuState.leaveMain = false;
        menuState.abortMain = false;
        menuState.dialogResult = Constants::DMR_NONE;

        if (!LoadSplashInternal(renderer)) {
            menuState.abortMain = true;

            return menuState;
        }

        menuState.itemCount = Constants::MainMenuItems::MMI_EXIT + 1;
        menuState.selectedItemIndex = Settings::SettingsData.CurrentSavegameIndex == 0
            ? Constants::MMI_NEWSINGLE
            : Constants::MMI_CUE;

        PrepareTextInternal(renderer);

        constexpr SDL_FRect srcRect{ 0,0, Constants::GodsPreferredWidth, Constants::GodsPreferredHight };
        constexpr SDL_FRect dstRect{ 0,0, Constants::GodsPreferredWidth, Constants::GodsPreferredHight };
        SDL_FRect outerMenuRect{ 150,45,340,390 };
        SDL_FRect titleRect{ 0,0,0,0 };
        SDL_FRect authorRect{ 0,0,0,0 };

        const auto titleTexture = Scripture::RenderText(
            renderer,
            &titleRect,
            Scripture::FONT_KEY_ALIEN,
            26,
            literalAlienMainMenuLabel,
            { 250,200,200,222 }
        );

        const auto authorTexture = Scripture::RenderText(
            renderer,
            &authorRect,
            Scripture::FONT_KEY_ALIEN,
            26,
            literalManufacturer,
            { 250,200,200,222 }
        );

        unsigned short frame = 0L;

        while (!menuState.leaveDialog) {
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
                menuState.abortMain = true;
                menuState.leaveDialog = true;

                break;
            }

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
            SDL_RenderFillRect(renderer, &outerMenuRect);
            SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
            if (!SDL_RenderRect(renderer, &outerMenuRect))
            {
                const auto drawRectError = SDL_GetError();

                ReportError("Failed to draw panel border", drawRectError);
                menuState.abortMain = true;
                menuState.leaveDialog = true;
            };

            DialogControls::DrawLabel(renderer, 286, 51, titleTexture, &titleRect);
            DialogControls::DrawLabel(renderer, 346, 451, authorTexture, &authorRect);

            const auto drawingTexture = Drawing::BeginRenderDrawing(renderer, Constants::GodsPreferredWidth, Constants::GodsPreferredHight);

            if (drawingTexture)
            {
                const auto drawingSink = Drawing::GetDrawingSink();
                constexpr int yStart = 94;
                constexpr int stride = 46;
                Constants::MainMenuItems itemToDraw = Constants::MMI_CUE;

                for (auto y = yStart; y < 400; y += stride)
                {
                    auto thisItem = itemToDraw == menuState.selectedItemIndex;

                    DialogControls::DrawButton(drawingSink, 195, y, 250, 42, thisItem ? Constants::CH_MAINMENU : Constants::CH_NONE);
                    if (thisItem)
                    {
                        DialogControls::DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
                        DialogControls::DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);
                    }

                    itemToDraw = static_cast<Constants::MainMenuItems>(itemToDraw + 1);
                }

                blooDot::Drawing::EndRenderDrawing(renderer, drawingTexture, nullptr);

                DialogControls::DrawLabel(renderer, 235, yStart + 0 * stride, continueTexture, &continueRect);
                DialogControls::DrawLabel(renderer, 235, yStart + 1 * stride, loadTexture, &loadRect);
                DialogControls::DrawLabel(renderer, 235, yStart + 2 * stride, singleTexture, &singleRect);
                DialogControls::DrawLabel(renderer, 235, yStart + 3 * stride, localMultiTexture, &localMultiRect);
                DialogControls::DrawLabel(renderer, 235, yStart + 4 * stride, creatorModeTexture, &creatorModeRect);
                DialogControls::DrawLabel(renderer, 235, yStart + 5 * stride, settingsTexture, &settingsRect);
                DialogControls::DrawLabel(renderer, 235, yStart + 6 * stride, quitTexture, &quitRect);
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
            ++frame;
        }

        return menuState;
    }

    bool LoadSplashInternal(SDL_Renderer* renderer) {
        SDL_FRect splashRect;
        splashTexture = Res::LoadPicture(renderer, CHUNK_KEY_SPLASH, &splashRect);

        return splashTexture;
    }

    void PrepareTextInternal(SDL_Renderer* renderer, const bool destroy)
    {
        Drawing::DestroyTexture(&continueTexture);
        Drawing::DestroyTexture(&loadTexture);
        Drawing::DestroyTexture(&singleTexture);
        Drawing::DestroyTexture(&localMultiTexture);
        Drawing::DestroyTexture(&creatorModeTexture);
        Drawing::DestroyTexture(&settingsTexture);
        Drawing::DestroyTexture(&quitTexture);

        if (!destroy)
        {
            continueTexture = Scripture::RenderText(renderer, &continueRect, Scripture::FONT_KEY_DIALOG, 23, literalMenuContinue, { 250, 230, 230, 245 });
            loadTexture = Scripture::RenderText(renderer, &loadRect, Scripture::FONT_KEY_DIALOG, 23, literalMenuLoad, { 250, 230, 230, 245 });
            singleTexture = Scripture::RenderText(renderer, &singleRect, Scripture::FONT_KEY_DIALOG, 23, literalMenuSingle, { 250, 230, 230, 245 });
            localMultiTexture = Scripture::RenderText(renderer, &localMultiRect, Scripture::FONT_KEY_DIALOG, 23, literalMenuLocalMulti, { 250, 230, 230, 245 });
            creatorModeTexture = Scripture::RenderText(renderer, &creatorModeRect, Scripture::FONT_KEY_DIALOG, 23, literalMenuMaker, { 250, 230, 230, 245 });
            settingsTexture = Scripture::RenderText(renderer, &settingsRect, Scripture::FONT_KEY_DIALOG, 23, literalMenuSettings, { 250, 230, 230, 245 });
            quitTexture = Scripture::RenderText(renderer, &quitRect, Scripture::FONT_KEY_DIALOG, 23, literalMenuExit, { 250, 230, 230, 245 });
        }
    }
}