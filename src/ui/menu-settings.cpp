#include "../../cairo/include/cairo.h"
#include "menu-settings.h"

#include "dialog-controls.h"
#include "drawing.h"
#include "scripture.h"
#include "menu-common.h"
#include "../../main.h"
#include "../../res/xlations.h"
#include "../state/settings.h"
#include "layout-constants.h"
#include "menu-settings-about.h"
#include "menu-settings-lang.h"

namespace blooDot::MenuSettings
{
	MenuCommon::MenuDialogInteraction menuState;

	SDL_Texture* backTexture = nullptr;
	SDL_Texture* screensizeTexture = nullptr;
	SDL_Texture* controlsTexture = nullptr;
	SDL_Texture* languageTexture = nullptr;
	SDL_Texture* helpTexture = nullptr;
	SDL_Texture* aboutTexture = nullptr;

	SDL_FRect backRect{ 0,0,0,0 };
	SDL_FRect screensizeRect{ 0,0,0,0 };
	SDL_FRect controlsRect{ 0,0,0,0 };
	SDL_FRect languageRect{ 0,0,0,0 };
	SDL_FRect helpRect{ 0,0,0,0 };
	SDL_FRect aboutRect{ 0,0,0,0 };

	bool MenuLoop(SDL_Renderer* renderer)
	{
		menuState.leaveDialog = false;
		menuState.itemCount = Constants::SettingsMenuItems::SMI_ABOUT + 1;
		menuState.selectedItemIndex = Constants::SettingsMenuItems::SMI_BACK;

		constexpr SDL_FRect outerMenuRect{ 150,45,340,390 };
		SDL_FRect titleRect{ 0,0,0,0 };

		const auto titleTexture = Scripture::RenderText(
			renderer,
			&titleRect,
			Scripture::FONT_KEY_ALIEN,
			Ui::MenuPanelTitleFontSize,
			literalAlienSettingsMenuLabel,
			Constants::AlienTextColor
		);

		PrepareTextInternal(renderer);

		unsigned short frame = 0;

		while (!menuState.leaveDialog)
		{
			blooDot::MenuCommon::HandleMenu(&menuState);
			if (menuState.leaveDialog)
			{
				return !menuState.leaveMain;
			}

			if (menuState.enterMenuItem)
			{
				if (!EnterAndHandleSettingsMenuInternal(renderer)) {
					return false;
				}

				if (menuState.selectedItemIndex == static_cast<int>(Constants::SettingsMenuItems::SMI_BACK)) {
					return true;
				}
			}

			if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = SDL_GetError();

				ReportError("Failed to clear the settings menu screen", clearError);
			}

			if (!MenuCommon::DrawMenuPanel(renderer, &outerMenuRect, titleTexture, &titleRect))
			{
				// succeed failing
				return true;
			};

			if (const auto drawingTexture = Drawing::BeginRenderDrawing(renderer, Constants::GodsPreferredWidth, Constants::GodsPreferredHight)) [[likely]]
			{
				const auto drawingSink = Drawing::GetDrawingSink();
				constexpr int yStart = 94;
				constexpr int stride = 46;
				constexpr int backGap = stride / 2;

				Constants::SettingsMenuItems itemToDraw = Constants::SMI_BACK;

				for (auto y = 94; y < 411; y += stride)
				{
					const auto thisItem = itemToDraw == menuState.selectedItemIndex;

					DialogControls::DrawButton(drawingSink, 195, y, 250, 42, thisItem ? Constants::CH_MAINMENU : Constants::CH_NONE);
					if (thisItem)
					{
						DialogControls::DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
						DialogControls::DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);
					}

					if (itemToDraw == Constants::SettingsMenuItems::SMI_BACK || itemToDraw == Constants::SettingsMenuItems::SMI_LANGUAGE)
					{
						y += backGap;
					}

					itemToDraw = static_cast<Constants::SettingsMenuItems>(itemToDraw + 1);
				}

				Drawing::EndRenderDrawing(renderer, drawingTexture, nullptr);
				DialogControls::DrawLabel(renderer, 235, yStart + 0 * stride + 0 * backGap, backTexture, &backRect);
				DialogControls::DrawLabel(renderer, 235, yStart + 1 * stride + 1 * backGap, screensizeTexture, &screensizeRect);
				DialogControls::DrawLabel(renderer, 235, yStart + 2 * stride + 1 * backGap, controlsTexture, &controlsRect);
				DialogControls::DrawLabel(renderer, 235, yStart + 3 * stride + 1 * backGap, languageTexture, &languageRect);
				DialogControls::DrawLabel(renderer, 235, yStart + 4 * stride + 2 * backGap, helpTexture, &helpRect);
				DialogControls::DrawLabel(renderer, 235, yStart + 5 * stride + 2 * backGap, aboutTexture, &aboutRect);
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		SDL_DestroyTexture(titleTexture);
		PrepareTextInternal(renderer, true);
		Settings::Save();

		return true;
	}

	void PrepareTextInternal(SDL_Renderer* renderer, bool destroy)
	{
		Drawing::DestroyTexture(&backTexture);
		Drawing::DestroyTexture(&screensizeTexture);
		Drawing::DestroyTexture(&controlsTexture);
		Drawing::DestroyTexture(&languageTexture);
		Drawing::DestroyTexture(&helpTexture);
		Drawing::DestroyTexture(&aboutTexture);

		if (!destroy)
		{
			backTexture = Scripture::RenderText(renderer, &backRect, Scripture::FONT_KEY_DIALOG,Ui::MenuItemCaptionFontSize, literalMenuBack, Constants::ButtonTextColor);
			screensizeTexture = Scripture::RenderText(renderer, &screensizeRect, Scripture::FONT_KEY_DIALOG, Ui::MenuItemCaptionFontSize, literalSettingsMenuScreensize, Constants::ButtonTextColor);
			controlsTexture = Scripture::RenderText(renderer, &controlsRect, Scripture::FONT_KEY_DIALOG, Ui::MenuItemCaptionFontSize, literalMenuControls, Constants::ButtonTextColor);
			languageTexture = Scripture::RenderText(renderer, &languageRect, Scripture::FONT_KEY_DIALOG, Ui::MenuItemCaptionFontSize, literalSettingsMenuLanguage, Constants::ButtonTextColor);
			helpTexture = Scripture::RenderText(renderer, &helpRect, Scripture::FONT_KEY_DIALOG, Ui::MenuItemCaptionFontSize, literalSettingsMenuHelp, Constants::ButtonTextColor);
			aboutTexture = Scripture::RenderText(renderer, &aboutRect, Scripture::FONT_KEY_DIALOG, Ui::MenuItemCaptionFontSize, literalSettingsMenuAbout, Constants::ButtonTextColor);
		}
	}

	bool EnterAndHandleSettingsMenuInternal(SDL_Renderer* renderer)
	{
		switch (menuState.selectedItemIndex)
		{
			/*
		case SettingsMenuItems::SMI_SCREENSIZE:
			_EnterAndHandleScreenSettings(renderer);
			break;
*/
		case Constants::SettingsMenuItems::SMI_LANGUAGE:
			return EnterAndHandleLanguageSettingsInternal(renderer);
/*
		case SettingsMenuItems::SMI_CONTROLS:
			_EnterAndHandleControlsSettings(renderer);
			break;

		case SettingsMenuItems::SMI_HELP:
			_EnterAndHandleHelp(renderer);
			break;
		*/
		case Constants::SettingsMenuItems::SMI_ABOUT:
			return EnterAndHandleAboutInternal(renderer);

		default:
			return true;
		}
	}

	// void _EnterAndHandleScreenSettings(SDL_Renderer* renderer)
	// {
	// 	blooDot::MenuSettingsScreen::ScreenSettingsMenuLoop(renderer);
	// }
	//
	bool EnterAndHandleLanguageSettingsInternal(SDL_Renderer* renderer)
	{
	 	if (MenuSettingsLang::LanguageSettingsMenuLoop(renderer)) {
	 		PrepareTextInternal(renderer);
	 		return true;
	 	}

	 	return false;
	}
	//
	// void _EnterAndHandleControlsSettings(SDL_Renderer* renderer)
	// {
	// 	blooDot::MenuSettingsControls::ControlsSettingsMenuLoop(renderer);
	// }
	//
	bool EnterAndHandleAboutInternal(SDL_Renderer* renderer)
	{
		return MenuSettingsAbout::MenuLoop(renderer);
	}
	//
	// void _EnterAndHandleHelp(SDL_Renderer* renderer)
	// {
	// 	blooDot::MenuSettingsHelp::HelpMenuLoop(renderer);
	// }
}
