#include "pch.h"
#include "menu-settings-lang.h"

#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo/cairo.h>
#include "xlations.h"
#include "settings.h"
#include "constants.h"
#include "sfx.h"
#include "menu-common.h"

constexpr int const bounceMargin = 10;
constexpr int const vignetteWidth = 250;
constexpr int const vignetteHeight = 220;
constexpr int const vignetteGap = 10;
constexpr int const vignetteCount = 6;

extern SettingsStruct Settings;
extern bool mainRunning;

namespace blooDot::MenuSettingsLang
{
	SDL_Event langSettingsMenuEvent;
	bool langSettingsMenuRunning = false;
	auto menuSelection = LangSettingsMenuItems::LSMI_CANCEL;
	auto selectedLanguage = UserInterfaceLanguages::UIL_AMERICAN;
	auto movingToLanguage = UserInterfaceLanguages::UIL_AMERICAN;
	SDL_Texture* slidingLangs;
	int sliderTextureWidth;
	int sliderOffsetLeft = bounceMargin;
	int slideSpeed = 0;
	int targetOffsetLeft = 0;

	bool LanguageSettingsMenuLoop(SDL_Renderer* renderer)
	{
		constexpr int const startY = 94;
		constexpr int const stride = 46;
		constexpr int backGap = stride / 2;
		constexpr int const sliderY = startY + stride + backGap;
		constexpr SDL_Rect const carouselDestRect = { 195,sliderY,vignetteWidth,vignetteHeight };

		_PrepareControls(renderer);
		langSettingsMenuRunning = true;
		selectedLanguage = ::Settings.SettingUserInterfaceLanguage;
		movingToLanguage = ::Settings.SettingUserInterfaceLanguage;
		sliderOffsetLeft = static_cast<int>(selectedLanguage) * vignetteWidth + bounceMargin;

		SDL_Rect outerMenuRect{ 150,45,340,390 };
		SDL_Rect titleRect{ 0,0,0,0 };
		SDL_Rect cancelRect{ 0,0,0,0 };
		SDL_Rect carouselSrcRect = { sliderOffsetLeft,0,vignetteWidth,vignetteHeight };

		const auto titleTexture = RenderText(
			renderer,
			&titleRect,
			FONT_KEY_ALIEN,
			26,
			literalAlienLangSettingsMenuLabel,
			AlienTextColor
		);

		/* menu item text */
		const auto cancelTexture = RenderText(
			renderer,
			&cancelRect,
			FONT_KEY_DIALOG,
			23,
			literalMenuCancel,
			ButtonTextColor
		);

		unsigned short frame = 0L;
		while (langSettingsMenuRunning)
		{
			while (SDL_PollEvent(&langSettingsMenuEvent) != 0)
			{
				switch (langSettingsMenuEvent.type)
				{
				case SDL_QUIT:
					mainRunning = false;
					langSettingsMenuRunning = false;
					break;

				case SDL_KEYDOWN:
					switch (langSettingsMenuEvent.key.keysym.scancode)
					{
					case SDL_SCANCODE_DOWN:
					case SDL_SCANCODE_KP_2:
					case SDL_SCANCODE_S:
					case SDL_SCANCODE_PAGEDOWN:
					case SDL_SCANCODE_END:
						if (menuSelection == LSMI_USER_INTERFACE_LANGUAGE)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = LSMI_USER_INTERFACE_LANGUAGE;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_UP:
					case SDL_SCANCODE_KP_8:
					case SDL_SCANCODE_W:
					case SDL_SCANCODE_PAGEUP:
					case SDL_SCANCODE_HOME:
						if (menuSelection == LSMI_CANCEL)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = LSMI_CANCEL;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_LEFT:
					case SDL_SCANCODE_KP_4:
					case SDL_SCANCODE_A:
						if (menuSelection == LSMI_CANCEL)
						{
							menuSelection = LSMI_USER_INTERFACE_LANGUAGE;
						}

						if (selectedLanguage == UserInterfaceLanguages::UIL_AMERICAN)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else if (movingToLanguage == selectedLanguage)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
							movingToLanguage = static_cast<UserInterfaceLanguages>(static_cast<int>(selectedLanguage) - 1);
						}
						else if (movingToLanguage != UserInterfaceLanguages::UIL_AMERICAN)
						{
							--movingToLanguage;
						}

						break;

					case SDL_SCANCODE_RIGHT:
					case SDL_SCANCODE_KP_6:
					case SDL_SCANCODE_D:
						if (menuSelection == LSMI_CANCEL)
						{
							menuSelection = LSMI_USER_INTERFACE_LANGUAGE;
						}

						if (selectedLanguage == UserInterfaceLanguages::UIL_UKRAINIAN)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else if (movingToLanguage == selectedLanguage)
						{
							movingToLanguage = static_cast<UserInterfaceLanguages>(static_cast<int>(selectedLanguage) + 1);
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}
						else if (movingToLanguage != UserInterfaceLanguages::UIL_UKRAINIAN)
						{
							++movingToLanguage;
						}

						break;

					case SDL_SCANCODE_RETURN:
					case SDL_SCANCODE_RETURN2:
					case SDL_SCANCODE_KP_ENTER:
					case SDL_SCANCODE_SPACE:
						if (menuSelection == LSMI_USER_INTERFACE_LANGUAGE)
						{
							::Settings.SettingUserInterfaceLanguage = movingToLanguage;
							blooDot::Settings::ApplyLanguage();
							langSettingsMenuRunning = false;
						}
						else if (menuSelection == LSMI_CANCEL)
						{
							langSettingsMenuRunning = false;
						}

						blooDot::Sfx::Play(SoundEffect::SFX_SELCONF);
						break;

					case SDL_SCANCODE_ESCAPE:
						langSettingsMenuRunning = false;
						break;
					}

					break;
				}
			}

			if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = IMG_GetError();
				ReportError("Failed to clear the language settings menu screen", clearError);
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
			SDL_RenderFillRect(renderer, &outerMenuRect);
			SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
			if (SDL_RenderDrawRect(renderer, &outerMenuRect) < 0)
			{
				const auto drawRectError = SDL_GetError();
				ReportError("Failed to draw language settings menu panel border", drawRectError);
				langSettingsMenuRunning = false;
			};

			DrawLabel(renderer, 286, 51, titleTexture, &titleRect);

			const auto drawingTexture = BeginRenderDrawing(renderer, 640, 480);
			if (drawingTexture) [[likely]]
			{
				auto const& drawingSink = GetDrawingSink();

				LangSettingsMenuItems itemToDraw = LSMI_CANCEL;
				for (auto y = startY; y < 200; y += stride)
				{
					DrawButton(
						drawingSink,
						195,
						y,
						vignetteWidth,
						itemToDraw > LSMI_CANCEL ? vignetteHeight : 42,
						itemToDraw == menuSelection ? CH_MAINMENU : CH_NONE
					);

					if (itemToDraw == menuSelection)
					{
						if (menuSelection == LSMI_CANCEL)
						{
							DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
							DrawChevron(drawingSink, 195 + vignetteWidth + 7, y + 21, true, frame);
						}
						else
						{
							DrawChevron(drawingSink, 195 - 9, y + 105, true, frame);
							DrawChevron(drawingSink, 195 + vignetteWidth + 9, y + 105, false, frame);
						}
					}

					if (itemToDraw == LSMI_CANCEL)
					{
						y += backGap;
					}

					itemToDraw = static_cast<LangSettingsMenuItems>(itemToDraw + 1);
				}

				EndRenderDrawing(renderer, drawingTexture, nullptr);

				DrawLabel(renderer, 235, startY + 0 * stride + 0 * backGap, cancelTexture, &cancelRect);

				/* render the carousel choice (and the sliding animation) */
				_AnimateCarousel();
				carouselSrcRect.x = sliderOffsetLeft;
				SDL_RenderCopy(renderer, slidingLangs, &carouselSrcRect, &carouselDestRect);
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		_Teardown();
		titleTexture&& [titleTexture] { SDL_DestroyTexture(titleTexture); return false; }();
		cancelTexture&& [cancelTexture] { SDL_DestroyTexture(cancelTexture); return false; }();

		return langSettingsMenuRunning;
	}

	void _AnimateCarousel()
	{
		if (movingToLanguage == selectedLanguage)
		{
			slideSpeed = 0;
			return;
		}

		if (slideSpeed == 0)
		{
			targetOffsetLeft = bounceMargin + static_cast<int>(movingToLanguage) * vignetteWidth;
			slideSpeed = 40;
		}

		auto direction = targetOffsetLeft < sliderOffsetLeft ? -1 : 1;
		sliderOffsetLeft += direction * slideSpeed;
		if (direction == 1)
		{
			if (sliderOffsetLeft >= targetOffsetLeft)
			{
				sliderOffsetLeft = targetOffsetLeft;
				selectedLanguage = movingToLanguage;
			}
		}
		else
		{
			if (sliderOffsetLeft <= targetOffsetLeft)
			{
				sliderOffsetLeft = targetOffsetLeft;
				selectedLanguage = movingToLanguage;
			}
		}
	}

	void _PrepareControls(SDL_Renderer* renderer)
	{
		sliderTextureWidth = vignetteCount * vignetteWidth + (vignetteCount - 1) * vignetteGap + 2 * bounceMargin;
		slidingLangs = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET,
			sliderTextureWidth,
			vignetteHeight
		);

		if (slidingLangs)
		{
			if (SDL_SetTextureBlendMode(slidingLangs, SDL_BLENDMODE_BLEND) < 0)
			{
				const auto carouselBlendmodeError = SDL_GetError();
				ReportError("Could not set blend mode of sliding texture", carouselBlendmodeError);
				SDL_DestroyTexture(slidingLangs);
				slidingLangs = NULL;

				return;
			}

			if (SDL_SetRenderTarget(renderer, slidingLangs) < 0)
			{
				const auto targetError = SDL_GetError();
				ReportError("Could not set sliding texture as the render target", targetError);
				return;
			}

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			_VignetteLabel(renderer, FONT_KEY_DIALOG_FAT, 28, 0, 30, literalSettingsLanguageEN);
			_VignetteLabel(renderer, FONT_KEY_DIALOG, 13, 0, 70, literalSettingsLanguageENDetails);
			_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 0, 190, literalSettingsLanguageENName);

			_VignetteLabel(renderer, FONT_KEY_DIALOG_FAT, 28, 1, 30, literalSettingsLanguageFI);
			_VignetteLabel(renderer, FONT_KEY_DIALOG, 13, 1, 70, literalSettingsLanguageFIDetails);
			_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 1, 190, literalSettingsLanguageFIName);

			_VignetteLabel(renderer, FONT_KEY_DIALOG_FAT, 28, 2, 30, literalSettingsLanguageDE);
			_VignetteLabel(renderer, FONT_KEY_DIALOG, 13, 2, 70, literalSettingsLanguageDEDetails);
			_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 2, 190, literalSettingsLanguageDEName);

			_VignetteLabel(renderer, FONT_KEY_DIALOG_FAT, 28, 3, 30, literalSettingsLanguageUA);
			_VignetteLabel(renderer, FONT_KEY_DIALOG, 13, 3, 70, literalSettingsLanguageUADetails);
			_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 3, 190, literalSettingsLanguageUAName);

			blooDot::MenuCommon::DrawIcon(renderer, CHUNK_KEY_LANG_FLAG_AM_PNG, 0, vignetteWidth, bounceMargin);
			blooDot::MenuCommon::DrawIcon(renderer, CHUNK_KEY_LANG_FLAG_FI_PNG, 1, vignetteWidth, bounceMargin);
			blooDot::MenuCommon::DrawIcon(renderer, CHUNK_KEY_LANG_FLAG_DE_PNG, 2, vignetteWidth, bounceMargin);
			blooDot::MenuCommon::DrawIcon(renderer, CHUNK_KEY_LANG_FLAG_UA_PNG, 3, vignetteWidth, bounceMargin);

			if (SDL_SetRenderTarget(renderer, NULL) < 0)
			{
				const auto restoreError = SDL_GetError();
				ReportError("Could not restore render target after rendering to sliding texture", restoreError);
				return;
			}

			return;
		}

		const auto newCarouselError = SDL_GetError();
		ReportError("Could not allocate sliding texture", newCarouselError);
	}

	void _VignetteLabel(SDL_Renderer* renderer, int font, int size, int vignetteIndex, int y, const char* text)
	{
		SDL_Rect rectLabel = { 0,0,0,0 };
		const auto textureLabel = RenderText(
			renderer,
			&rectLabel,
			font,
			size,
			text,
			{ 250, 230, 230, 245 }
		);

		CenterLabel(
			renderer,
			bounceMargin + vignetteIndex * vignetteWidth + vignetteWidth / 2,
			y,
			textureLabel,
			&rectLabel
		);

		SDL_DestroyTexture(textureLabel);
	}

	void _Teardown()
	{
		DestroyTexture(&slidingLangs);
	}
}