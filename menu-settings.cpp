#include "pch.h"
#include "menu-settings.h"
#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo.h>
#include "xlations.h"
#include "settings.h"
#include "constants.h"
#include "sfx.h"

extern SettingsStruct Settings;
extern bool mainRunning;

namespace blooDot::MenuSettings
{
	SDL_Event menuEvent;
	bool menuRunning = false;
	SettingsMenuItems menuSelection = SMI_BACK;

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
			while (SDL_PollEvent(&menuEvent) != 0)
			{
				switch (menuEvent.type)
				{
				case SDL_QUIT:
					mainRunning = false;
					menuRunning = false;
					break;

				case SDL_KEYDOWN:
					switch (menuEvent.key.keysym.scancode)
					{
					case SDL_SCANCODE_UP:
					case SDL_SCANCODE_KP_8:
					case SDL_SCANCODE_W:
						if (menuSelection == SMI_BACK)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = static_cast<SettingsMenuItems>(menuSelection - 1);
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_DOWN:
					case SDL_SCANCODE_KP_2:
					case SDL_SCANCODE_S:
						if (menuSelection == SMI_ABOUT)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = static_cast<SettingsMenuItems>(menuSelection + 1);
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_PAGEDOWN:
					case SDL_SCANCODE_END:
						if (menuSelection == SMI_ABOUT)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = SMI_ABOUT;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_PAGEUP:
					case SDL_SCANCODE_HOME:
						if (menuSelection == SMI_BACK)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = SMI_BACK;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_RETURN:
					case SDL_SCANCODE_RETURN2:
					case SDL_SCANCODE_KP_ENTER:
					case SDL_SCANCODE_SPACE:
						blooDot::Sfx::Play(SoundEffect::SFX_SELCONF);
						menuRunning = _EnterAndHandleSettingsMenu(renderer);
						break;

					case SDL_SCANCODE_ESCAPE:
						menuRunning = false;
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
				menuRunning = false;
			};

			DrawLabel(renderer, 286, 54, titleTexture, &titleRect);

			const auto drawingTexture = BeginRenderDrawing(renderer, GodsPreferredWidth, GodsPreferredHight);
			if (drawingTexture) [[likely]]
			{
				const auto drawingSink = GetDrawingSink();
				const int stride = 46;
				const int backGap = stride / 2;
				SettingsMenuItems itemToDraw = SMI_BACK;
				for (auto y = 94; y < 411; y += stride)
				{
					DrawButton(drawingSink, 195, y, 250, 42, itemToDraw == menuSelection);
					if (itemToDraw == menuSelection)
					{
						DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
						DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);
					}

					if (itemToDraw == SMI_BACK || itemToDraw == SMI_LANGUAGE)
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
				DrawLabel(renderer, 235, 100 + 4 * stride + 2 * backGap, helpTexture, &helpRect);
				DrawLabel(renderer, 235, 100 + 5 * stride + 2 * backGap, aboutTexture, &aboutRect);
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
		backTexture && [] { SDL_DestroyTexture(backTexture); return false; }();
		screensizeTexture && [] { SDL_DestroyTexture(screensizeTexture); return false; }();
		controlsTexture && [] { SDL_DestroyTexture(controlsTexture); return false; }();
		languageTexture && [] { SDL_DestroyTexture(languageTexture); return false; }();
		helpTexture && [] { SDL_DestroyTexture(helpTexture); return false; }();
		aboutTexture && [] { SDL_DestroyTexture(aboutTexture); return false; }();

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
		switch (menuSelection)
		{
		case SMI_SCREENSIZE:
			_EnterAndHandleScreenSettings(renderer);
			break;

		case SMI_LANGUAGE:
			_EnterAndHandleLanguageSettings(renderer);
			break;

		case SMI_HELP:
			break;

		case SMI_ABOUT:
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
}