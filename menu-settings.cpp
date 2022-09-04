#include "pch.h"
#include "menu-settings.h"
#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo.h>
#include "xlations.h"
#include "settings.h"
#include "constants.h"

extern SettingsStruct Settings;
extern bool mainRunning;

SDL_Event settingsMenuEvent;
bool settingsMenuRunning = false;
SettingsMenuItems menuSelection = SMI_BACK;

bool SettingsMenuLoop(SDL_Renderer* renderer)
{
	settingsMenuRunning = true;

	SDL_Rect outerMenuRect{ 150,45,340,390 };
	SDL_Rect titleRect{ 0,0,0,0 };
	SDL_Rect backRect{ 0,0,0,0 };
	SDL_Rect screensizeRect{ 0,0,0,0 };
	SDL_Rect controlsRect{ 0,0,0,0 };
	SDL_Rect languageRect{ 0,0,0,0 };

	const auto titleTexture = RenderText(
		renderer,
		&titleRect,
		FONT_KEY_ALIEN,
		26,
		literalAlienSettingsMenuLabel,
		AlienTextColor
	);

	/* menu item text */
	const auto backTexture = RenderText(
		renderer,
		&backRect,
		FONT_KEY_DIALOG,
		23,
		literalMenuBack,
		ButtonTextColor
	);

	const auto screensizeTexture = RenderText(
		renderer,
		&screensizeRect,
		FONT_KEY_DIALOG,
		23,
		literalSettingsMenuScreensize,
		ButtonTextColor
	);

	const auto controlsTexture = RenderText(
		renderer,
		&controlsRect,
		FONT_KEY_DIALOG,
		23,
		literalSettingsMenuControls,
		ButtonTextColor
	);

	const auto languageTexture = RenderText(
		renderer,
		&languageRect,
		FONT_KEY_DIALOG,
		23,
		literalSettingsMenuLanguage,
		ButtonTextColor
	);

	unsigned short frame = 0;
	while (settingsMenuRunning && mainRunning)
	{
		while (SDL_PollEvent(&settingsMenuEvent) != 0)
		{
			switch (settingsMenuEvent.type)
			{
			case SDL_QUIT:
				mainRunning = false;
				settingsMenuRunning = false;
				break;

			case SDL_KEYDOWN:
				switch (settingsMenuEvent.key.keysym.scancode)
				{
				case SDL_SCANCODE_UP:
				case SDL_SCANCODE_KP_8:
				case SDL_SCANCODE_W:
					if (menuSelection != SMI_BACK)
					{
						menuSelection = static_cast<SettingsMenuItems>(menuSelection - 1);
					}

					break;

				case SDL_SCANCODE_DOWN:
				case SDL_SCANCODE_KP_2:
				case SDL_SCANCODE_S:
					if (menuSelection != SMI_LANGUAGE)
					{
						menuSelection = static_cast<SettingsMenuItems>(menuSelection + 1);
					}

					break;

				case SDL_SCANCODE_PAGEDOWN:
				case SDL_SCANCODE_END:
					menuSelection = SMI_LANGUAGE;
					break;

				case SDL_SCANCODE_PAGEUP:
				case SDL_SCANCODE_HOME:
					menuSelection = SMI_BACK;
					break;

				case SDL_SCANCODE_RETURN:
				case SDL_SCANCODE_RETURN2:
				case SDL_SCANCODE_KP_ENTER:
				case SDL_SCANCODE_SPACE:
					settingsMenuRunning = _EnterAndHandleSettingsMenu(renderer);
					break;

				case SDL_SCANCODE_ESCAPE:
					settingsMenuRunning = false;
					break;
				}

				break;
			}
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
			settingsMenuRunning = false;
		};
		
		DrawLabel(renderer, 286, 54, titleTexture, &titleRect);

		const auto drawingTexture = BeginRenderDrawing(renderer, GodsPreferredWidth, GodsPreferredHight);
		if (drawingTexture) [[likely]]
		{
			const auto drawingSink = GetDrawingSink();
			const int stride = 46;
			const int backGap = stride / 2;
			SettingsMenuItems itemToDraw = SMI_BACK;
			for (auto y = 94; y < 296; y += stride)
			{
				DrawButton(drawingSink, 195, y, 250, 42, itemToDraw == menuSelection);
				if (itemToDraw == menuSelection)
				{
					DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
					DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);
				}

				if(itemToDraw == SMI_BACK) 
				{
					y += backGap;
				}

				itemToDraw = static_cast<SettingsMenuItems>(itemToDraw + 1);
			}

			EndRenderDrawing(renderer, drawingTexture);

			DrawLabel(renderer, 235, 100 + 0 * stride + 0 * backGap, backTexture, &backRect);
			DrawLabel(renderer, 235, 100 + 1 * stride + 1 * backGap, screensizeTexture, &screensizeRect);
			DrawLabel(renderer, 235, 100 + 2 * stride + 1 * backGap, controlsTexture, &controlsRect);
			DrawLabel(renderer, 235, 100 + 3 * stride + 1 * backGap, languageTexture, &languageRect);
		}

		SDL_RenderPresent(renderer);
		SDL_Delay(16);
		++frame;
	}

	SDL_DestroyTexture(titleTexture);
	SDL_DestroyTexture(backTexture);
	SDL_DestroyTexture(screensizeTexture);
	SDL_DestroyTexture(controlsTexture);
	SDL_DestroyTexture(languageTexture);

	return settingsMenuRunning;
}

bool _EnterAndHandleSettingsMenu(SDL_Renderer* renderer)
{
	switch (menuSelection)
	{
	case SMI_SCREENSIZE:
		_EnterAndHandleScreenSettings(renderer);
		break;

	case SMI_LANGUAGE:
		_EnterAndHandleLanguageSettings(renderer);
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
}