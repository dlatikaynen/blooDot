#include <cairo/cairo.h>
#include "pch.h"
#include "menu-settings.h"
#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include "xlations.h"
#include "settings.h"
#include "constants.h"
#include "sfx.h"
#include "menu-common.h"
#include "menu-settings-about.h"
#include "menu-settings-help.h"

extern SettingsStruct Settings;
extern bool mainRunning;

namespace blooDot::MenuSettings
{
	MenuDialogInteraction menuState;
	bool menuRunning = false;

	SDL_Texture* backTexture = NULL;
	SDL_Texture* screensizeTexture = NULL;
	SDL_Texture* controlsTexture = NULL;
	SDL_Texture* languageTexture = NULL;
	SDL_Texture* helpTexture = NULL;
	SDL_Texture* aboutTexture = NULL;

	SDL_Rect backRect{ 0,0,0,0 };
	SDL_Rect screensizeRect{ 0,0,0,0 };
	SDL_Rect controlsRect{ 0,0,0,0 };
	SDL_Rect languageRect{ 0,0,0,0 };
	SDL_Rect helpRect{ 0,0,0,0 };
	SDL_Rect aboutRect{ 0,0,0,0 };

	bool MenuLoop(SDL_Renderer* renderer)
	{
		menuRunning = true;
		menuState.itemCount = SettingsMenuItems::SMI_ABOUT + 1;
		menuState.selectedItemIndex = SettingsMenuItems::SMI_BACK;

		SDL_Rect outerMenuRect{ 150,45,340,390 };
		SDL_Rect titleRect{ 0,0,0,0 };

		const auto titleTexture = RenderText(
			renderer,
			&titleRect,
			FONT_KEY_ALIEN,
			26,
			literalAlienSettingsMenuLabel,
			AlienTextColor
		);

		_PrepareText(renderer);

		unsigned short frame = 0;
		while (menuRunning && mainRunning)
		{
			blooDot::MenuCommon::HandleMenu(&menuState);
			if (menuState.leaveDialog)
			{
				menuRunning = false;
			}
			else if (menuState.enterMenuItem)
			{
				menuRunning = _EnterAndHandleSettingsMenu(renderer);
			}

			if (menuState.leaveMain)
			{
				mainRunning = false;
			}

			if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = IMG_GetError();
				ReportError("Failed to clear the settings menu screen", clearError);
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
			SDL_RenderFillRect(renderer, &outerMenuRect);
			SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
			if (SDL_RenderDrawRect(renderer, &outerMenuRect) < 0)
			{
				const auto drawRectError = SDL_GetError();
				ReportError("Failed to draw settings menu panel border", drawRectError);
				menuRunning = false;
			};

			DrawLabel(renderer, 286, 51, titleTexture, &titleRect);

			const auto drawingTexture = BeginRenderDrawing(renderer, GodsPreferredWidth, GodsPreferredHight);
			if (drawingTexture) [[likely]]
			{
				const auto drawingSink = GetDrawingSink();
				const int yStart = 94;
				const int stride = 46;
				const int backGap = stride / 2;
				SettingsMenuItems itemToDraw = SMI_BACK;
				for (auto y = 94; y < 411; y += stride)
				{
					const auto thisItem = itemToDraw == menuState.selectedItemIndex;
					DrawButton(drawingSink, 195, y, 250, 42, thisItem);
					if (thisItem)
					{
						DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
						DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);
					}

					if (itemToDraw == SettingsMenuItems::SMI_BACK || itemToDraw == SettingsMenuItems::SMI_LANGUAGE)
					{
						y += backGap;
					}

					itemToDraw = static_cast<SettingsMenuItems>(itemToDraw + 1);
				}

				EndRenderDrawing(renderer, drawingTexture, nullptr);

				DrawLabel(renderer, 235, yStart + 0 * stride + 0 * backGap, backTexture, &backRect);
				DrawLabel(renderer, 235, yStart + 1 * stride + 1 * backGap, screensizeTexture, &screensizeRect);
				DrawLabel(renderer, 235, yStart + 2 * stride + 1 * backGap, controlsTexture, &controlsRect);
				DrawLabel(renderer, 235, yStart + 3 * stride + 1 * backGap, languageTexture, &languageRect);
				DrawLabel(renderer, 235, yStart + 4 * stride + 2 * backGap, helpTexture, &helpRect);
				DrawLabel(renderer, 235, yStart + 5 * stride + 2 * backGap, aboutTexture, &aboutRect);
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		SDL_DestroyTexture(titleTexture);
		_PrepareText(renderer, true);
		blooDot::Settings::Save();

		return menuRunning;
	}

	void _PrepareText(SDL_Renderer* renderer, bool destroy)
	{
		DestroyTexture(&backTexture);
		DestroyTexture(&screensizeTexture);
		DestroyTexture(&controlsTexture);
		DestroyTexture(&languageTexture);
		DestroyTexture(&helpTexture);
		DestroyTexture(&aboutTexture);

		if (!destroy)
		{
			backTexture = RenderText(renderer, &backRect, FONT_KEY_DIALOG, 23, literalMenuBack, ButtonTextColor);
			screensizeTexture = RenderText(renderer, &screensizeRect, FONT_KEY_DIALOG, 23, literalSettingsMenuScreensize, ButtonTextColor);
			controlsTexture = RenderText(renderer, &controlsRect, FONT_KEY_DIALOG, 23, literalMenuControls, ButtonTextColor);
			languageTexture = RenderText(renderer, &languageRect, FONT_KEY_DIALOG, 23, literalSettingsMenuLanguage, ButtonTextColor);
			helpTexture = RenderText(renderer, &helpRect, FONT_KEY_DIALOG, 23, literalSettingsMenuHelp, ButtonTextColor);
			aboutTexture = RenderText(renderer, &aboutRect, FONT_KEY_DIALOG, 23, literalSettingsMenuAbout, ButtonTextColor);
		}
	}

	bool _EnterAndHandleSettingsMenu(SDL_Renderer* renderer)
	{
		switch (menuState.selectedItemIndex)
		{
		case SettingsMenuItems::SMI_SCREENSIZE:
			_EnterAndHandleScreenSettings(renderer);
			break;

		case SettingsMenuItems::SMI_LANGUAGE:
			_EnterAndHandleLanguageSettings(renderer);
			break;

		case SettingsMenuItems::SMI_HELP:
			_EnterAndHandleHelp(renderer);
			break;

		case SettingsMenuItems::SMI_ABOUT:
			_EnterAndHandleAbout(renderer);
			break;

		default:
			return false;
		}

		return true;
	}

	void _EnterAndHandleScreenSettings(SDL_Renderer* renderer)
	{
		blooDot::MenuSettingsScreen::ScreenSettingsMenuLoop(renderer);
	}

	void _EnterAndHandleLanguageSettings(SDL_Renderer* renderer)
	{
		blooDot::MenuSettingsLang::LanguageSettingsMenuLoop(renderer);
		_PrepareText(renderer);
	}

	void _EnterAndHandleAbout(SDL_Renderer* renderer)
	{
		blooDot::MenuSettingsAbout::MenuLoop(renderer);
	}

	void _EnterAndHandleHelp(SDL_Renderer* renderer)
	{
		blooDot::MenuSettingsHelp::HelpMenuLoop(renderer);
	}
}