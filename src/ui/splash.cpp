#include "splash.h"

#include <random>

#include "dialog-controls.h"
#include "drawing.h"
#include "layout-constants.h"
#include "menu-common.h"
#include "menu-settings.h"
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
    int backgroundSpeedX = 0;
    int backgroundSpeedY = 0;
    constexpr int backdropHorz = 1280 - Constants::GodsPreferredWidth;
    constexpr int backdropVert = 720 - Constants::GodsPreferredHight;
    auto backgroundAnimDelay = Ui::BackgroundAnimInitialDelay;
    std::default_random_engine generator; // NOLINT(*-msc51-cpp)
    std::uniform_int_distribution distribution(0, 3);
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
        menuState.isTopLevel = true;
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

        SDL_FRect srcRect{ 0,0, Constants::GodsPreferredWidth, Constants::GodsPreferredHight };
        constexpr SDL_FRect dstRect{ 0,0, Constants::GodsPreferredWidth, Constants::GodsPreferredHight };
        constexpr SDL_FRect outerMenuRect{ 150,45,340,390 };
        SDL_FRect titleRect{ 0,0,0,0 };
        SDL_FRect authorRect{ 0,0,0,0 };

        const auto titleTexture = Scripture::RenderText(
            renderer,
            &titleRect,
            Scripture::FONT_KEY_ALIEN,
            Ui::MenuPanelTitleFontSize,
            literalAlienMainMenuLabel,
            Ui::CaptionColor
        );

        const auto authorTexture = Scripture::RenderText(
            renderer,
            &authorRect,
            Scripture::FONT_KEY_ALIEN,
            Ui::MenuPanelTitleFontSize,
            literalManufacturer,
            Ui::CaptionColor
        );

        unsigned short frame = 0L;

        while (!menuState.leaveDialog) {
            MenuCommon::HandleMenu(&menuState);
            if (menuState.enterMenuItem)
            {
                if (!EnterAndHandleMenuInternal(renderer)) {
                    menuState.leaveMain = true;
                    menuState.leaveDialog = true;
                }
            } else if (menuState.leaveDialog) {
                /* escaping on the splash dialog is equivalent to quit
                 * (there is no level of menu navigation above splash) */
                menuState.leaveMain = true;
            }

            BounceInternal(&srcRect);

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
            if (!MenuCommon::DrawMenuPanel(renderer, &outerMenuRect, titleTexture, &titleRect)) {
                menuState.abortMain = true;
                menuState.leaveDialog = true;
            }

            DialogControls::DrawLabel(renderer, 346, 451, authorTexture, &authorRect);

            if (const auto drawingTexture = Drawing::BeginRenderDrawing(renderer, Constants::GodsPreferredWidth, Constants::GodsPreferredHight))
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

                Drawing::EndRenderDrawing(renderer, drawingTexture, nullptr);
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

        // cleanup
        Drawing::DestroyTexture(&splashTexture);
        if (titleTexture != nullptr)
        {
            SDL_DestroyTexture(titleTexture);
        }

        if (authorTexture != nullptr)
        {
            SDL_DestroyTexture(authorTexture);
        }

        PrepareTextInternal(renderer, true);

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
            continueTexture = Scripture::RenderText(renderer, &continueRect, Scripture::FONT_KEY_DIALOG, Ui::MenuItemCaptionFontSize, literalMenuContinue, Ui::MenuItemCaptionColor);
            loadTexture = Scripture::RenderText(renderer, &loadRect, Scripture::FONT_KEY_DIALOG, Ui::MenuItemCaptionFontSize, literalMenuLoad, Ui::MenuItemCaptionColor);
            singleTexture = Scripture::RenderText(renderer, &singleRect, Scripture::FONT_KEY_DIALOG, Ui::MenuItemCaptionFontSize, literalMenuSingle, Ui::MenuItemCaptionColor);
            localMultiTexture = Scripture::RenderText(renderer, &localMultiRect, Scripture::FONT_KEY_DIALOG, Ui::MenuItemCaptionFontSize, literalMenuLocalMulti, Ui::MenuItemCaptionColor);
            creatorModeTexture = Scripture::RenderText(renderer, &creatorModeRect, Scripture::FONT_KEY_DIALOG, Ui::MenuItemCaptionFontSize, literalMenuMaker, Ui::MenuItemCaptionColor);
            settingsTexture = Scripture::RenderText(renderer, &settingsRect, Scripture::FONT_KEY_DIALOG, Ui::MenuItemCaptionFontSize, literalMenuSettings, Ui::MenuItemCaptionColor);
            quitTexture = Scripture::RenderText(renderer, &quitRect, Scripture::FONT_KEY_DIALOG, Ui::MenuItemCaptionFontSize, literalMenuExit, Ui::MenuItemCaptionColor);
        }
    }

    void BounceInternal(SDL_FRect* srcRect)
    {
        if (backgroundSpeedX == 0)
        {
            AssignNewSpeedInternal(&backgroundSpeedX);
        }

        if (backgroundSpeedY == 0)
        {
            AssignNewSpeedInternal(&backgroundSpeedY);
        }

        if (backgroundAnimDelay > 0)
        {
            --backgroundAnimDelay;

            return;
        }

        backgroundPosX += static_cast<double>(backgroundSpeedX) * 0.5;
        backgroundPosY += static_cast<double>(backgroundSpeedY) * 0.5;

        if (backgroundPosX < 0)
        {
            backgroundPosX = 0;
            AssignNewSpeedInternal(&backgroundSpeedX);
            backgroundSpeedX = abs(backgroundSpeedX);
            DelayBackgroundAnimInternal();
        }
        else if (backgroundPosX > backdropHorz)
        {
            backgroundPosX = backdropHorz;
            AssignNewSpeedInternal(&backgroundSpeedX);
            backgroundSpeedX = -abs(backgroundSpeedX);
            DelayBackgroundAnimInternal();
        }

        if (backgroundPosY < 0)
        {
            backgroundPosY = 0;
            AssignNewSpeedInternal(&backgroundSpeedY);
            backgroundSpeedY = abs(backgroundSpeedY);
            DelayBackgroundAnimInternal();
        }
        else if (backgroundPosY > backdropVert)
        {
            backgroundPosY = backdropVert;
            AssignNewSpeedInternal(&backgroundSpeedY);
            backgroundSpeedY = -abs(backgroundSpeedY);
            DelayBackgroundAnimInternal();
        }

        srcRect->x = static_cast<float>(round(backgroundPosX));
        srcRect->y = static_cast<float>(round(backgroundPosY));
    }

    void AssignNewSpeedInternal(int* speed)
    {
        if (const auto value = distribution(generator); value < 2)
        {
            /* -2, -1 */
            (*speed) = value - 2;
        }
        else
        {
            /* 1, 2 */
            (*speed) = value - 1;
        }
    }

    void DelayBackgroundAnimInternal()
    {
        const auto random = distribution(generator);

        backgroundAnimDelay = Ui::BackgroundAnimDelay * (random + 1);
    }

    bool EnterAndHandleMenuInternal(SDL_Renderer* renderer)
    {
        switch (menuState.selectedItemIndex)
        {
            case Constants::MMI_CUE:
//                if (!_HandleContinue(renderer))
//                {
                    menuState.leaveDialog = true;
                    menuState.dialogResult = Constants::DMR_LAUNCH_MAKER;
//               }

                return true;

            // case MMI_LOAD:
            //     if (!_HandleLoad(renderer))
            //     {
            //         splashRunning = false;
            //     }
            //
            //     break;
            //
            // case MMI_NEWSINGLE:
            // case MMI_NEWMULTI:
            //     if (!_HandleNew(renderer))
            //     {
            //         splashRunning = false;
            //     }
            //
            //     break;
            //
            // case MMI_CREATORMODE:
            //     isCreatorMode = true;
            //     if (!_EnterAndHandleCreatorMode(renderer))
            //     {
            //         splashRunning = false;
            //     }
            //
            //     break;

            case Constants::MMI_SETTINGS:
                 return EnterAndHandleSettingsInternal(renderer);

            case Constants::MMI_EXIT:
                menuState.leaveMain = true;
                menuState.leaveDialog = true;
                return false;

            default:
                break;
        }

        return true;
    }

    bool EnterAndHandleSettingsInternal(SDL_Renderer* renderer)
    {
        const auto& lcidBefore = std::string(literalLCID);

        if (!MenuSettings::MenuLoop(renderer)) {
            return false;
        }

        // because language might have changed
        if (const auto& lcidAfter = std::string(literalLCID); lcidAfter != lcidBefore)
        {
            PrepareTextInternal(renderer);
        }

        return true;
    }
}