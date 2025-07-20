#include "menu-settings-lang.h"

#include "dialog-controls.h"
#include "drawing.h"
#include "scripture.h"
#include "menu-common.h"
#include "../state/settings.h"
#include "layout-constants.h"
#include "../../res/xlations.h"
#include "../../res/chunk-constants.h"
#include "../../main.h"
#include "../../src/snd/sfx.h"

constexpr float bounceMargin = 10;
constexpr float vignetteWidth = 250;
constexpr int vignetteHeight = 220;
constexpr int vignetteGap = 10;
constexpr int vignetteCount = 6;

namespace blooDot::MenuSettingsLang
{
	MenuCommon::MenuDialogInteraction menuState;
	SDL_Event controlsSettingsMenuEvent;
	auto menuSelection = Constants::LangSettingsMenuItems::LSMI_CANCEL;
	auto selectedLanguage = Constants::UserInterfaceLanguages::UIL_AMERICAN;
	auto movingToLanguage = Constants::UserInterfaceLanguages::UIL_AMERICAN;
	SDL_Texture* slidingLangs;
	int sliderTextureWidth;
	float sliderOffsetLeft = bounceMargin;
	float slideSpeed = 0;
	float targetOffsetLeft = 0;

	Constants::DialogMenuResult LanguageSettingsMenuLoop(SDL_Renderer* renderer)
	{
		constexpr int startY = 94;
		constexpr int stride = 46;
		constexpr int backGap = stride / 2;
		constexpr int sliderY = startY + stride + backGap;
		constexpr SDL_FRect carouselDestRect = { 195,sliderY,vignetteWidth,vignetteHeight };

		menuState.leaveDialog = false;
		menuState.dialogResult = Constants::DMR_CANCEL;
		menuState.itemCount = Constants::LangSettingsMenuItems::LSMI_USER_INTERFACE_LANGUAGE + 1;
		menuState.selectedItemIndex = Constants::LangSettingsMenuItems::LSMI_CANCEL;

		PrepareControlsInternal(renderer);
		selectedLanguage = Settings::SettingsData.SettingUserInterfaceLanguage;
		movingToLanguage = Settings::SettingsData.SettingUserInterfaceLanguage;
		sliderOffsetLeft = static_cast<float>(selectedLanguage) * vignetteWidth + bounceMargin;

		constexpr SDL_FRect outerMenuRect{ 150,45,340,390 };
		SDL_FRect titleRect{ 0,0,0,0 };
		SDL_FRect cancelRect{ 0,0,0,0 };
		SDL_FRect carouselSrcRect = { sliderOffsetLeft,0,vignetteWidth,vignetteHeight };

		const auto titleTexture = Scripture::RenderText(
			renderer,
			&titleRect,
			Scripture::FONT_KEY_ALIEN,
			Ui::MenuPanelTitleFontSize,
			literalAlienLangSettingsMenuLabel,
			Constants::AlienTextColor
		);

		/* menu item text */
		const auto cancelTexture = Scripture::RenderText(
			renderer,
			&cancelRect,
			Scripture::FONT_KEY_DIALOG,
			Ui::MenuItemCaptionFontSize,
			literalMenuCancel,
			Constants::ButtonTextColor
		);

		unsigned short frame = 0L;
		while (!menuState.leaveDialog)
		{
			while (SDL_PollEvent(&controlsSettingsMenuEvent) != 0)
			{
				// TODO: use shared carousel movement event handling from menu-common
				switch (controlsSettingsMenuEvent.type)
				{
				case SDL_EVENT_QUIT:
					menuState.leaveDialog = true;
					menuState.leaveMain = true;
					menuState.dialogResult = Constants::DMR_QUIT_MAIN;

					break;

				case SDL_EVENT_KEY_DOWN:
					switch (controlsSettingsMenuEvent.key.scancode)
					{
					case SDL_SCANCODE_DOWN:
					case SDL_SCANCODE_KP_2:
					case SDL_SCANCODE_S:
					case SDL_SCANCODE_PAGEDOWN:
					case SDL_SCANCODE_END:
						if (menuSelection == Constants::LSMI_USER_INTERFACE_LANGUAGE)
						{
							blooDot::Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = Constants::LSMI_USER_INTERFACE_LANGUAGE;
							blooDot::Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_UP:
					case SDL_SCANCODE_KP_8:
					case SDL_SCANCODE_W:
					case SDL_SCANCODE_PAGEUP:
					case SDL_SCANCODE_HOME:
						if (menuSelection == Constants::LSMI_CANCEL)
						{
							blooDot::Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = Constants::LSMI_CANCEL;
							blooDot::Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_LEFT:
					case SDL_SCANCODE_KP_4:
					case SDL_SCANCODE_A:
						if (menuSelection == Constants::LSMI_CANCEL)
						{
							menuSelection = Constants::LSMI_USER_INTERFACE_LANGUAGE;
						}

						if (selectedLanguage == Constants::UserInterfaceLanguages::UIL_AMERICAN)
						{
							blooDot::Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
						}
						else if (movingToLanguage == selectedLanguage)
						{
							blooDot::Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
							movingToLanguage = static_cast<Constants::UserInterfaceLanguages>(static_cast<int>(selectedLanguage) - 1);
						}
						else if (movingToLanguage != Constants::UserInterfaceLanguages::UIL_AMERICAN)
						{
							--movingToLanguage;
						}

						break;

					case SDL_SCANCODE_RIGHT:
					case SDL_SCANCODE_KP_6:
					case SDL_SCANCODE_D:
						if (menuSelection == Constants::LSMI_CANCEL)
						{
							menuSelection = Constants::LSMI_USER_INTERFACE_LANGUAGE;
						}

						if (selectedLanguage == Constants::UserInterfaceLanguages::UIL_UKRAINIAN)
						{
							blooDot::Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
						}
						else if (movingToLanguage == selectedLanguage)
						{
							movingToLanguage = static_cast<Constants::UserInterfaceLanguages>(static_cast<int>(selectedLanguage) + 1);
							blooDot::Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
						}
						else if (movingToLanguage != Constants::UserInterfaceLanguages::UIL_UKRAINIAN)
						{
							++movingToLanguage;
						}

						break;

					case SDL_SCANCODE_RETURN:
					case SDL_SCANCODE_RETURN2:
					case SDL_SCANCODE_KP_ENTER:
					case SDL_SCANCODE_SPACE:
						if (menuSelection == Constants::LSMI_USER_INTERFACE_LANGUAGE)
						{
							Settings::SettingsData.SettingUserInterfaceLanguage = movingToLanguage;
							Settings::ApplyLanguage();
							menuState.dialogResult = Constants::DMR_OK;
							menuState.leaveDialog = true;
						}
						else if (menuSelection == Constants::LSMI_CANCEL)
						{
							menuState.leaveDialog = true;
						}

						Sfx::Play(Sfx::SoundEffect::SFX_SELCONF);

						break;

					case SDL_SCANCODE_ESCAPE:
						menuState.leaveDialog = true;
						break;

					default:
							break;
					}

					break;

				default:
					break;
				}
			}

			if (!SDL_RenderClear(renderer))
			{
				const auto clearError = SDL_GetError();

				ReportError("Failed to clear the language settings menu screen", clearError);
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			if (!MenuCommon::DrawMenuPanel(renderer, &outerMenuRect, titleTexture, &titleRect))
			{
				return Constants::DMR_CANCEL;
			}

			if (const auto drawingTexture = Drawing::BeginRenderDrawing(renderer, 640, 480)) [[likely]]
			{
				auto const& drawingSink = Drawing::GetDrawingSink();

				Constants::LangSettingsMenuItems itemToDraw = Constants::LSMI_CANCEL;
				for (auto y = startY; y < 200; y += stride)
				{
					DialogControls::DrawButton(
						drawingSink,
						195,
						y,
						vignetteWidth,
						itemToDraw > Constants::LSMI_CANCEL ? vignetteHeight : 42,
						itemToDraw == menuSelection ? Constants::CH_MAINMENU : Constants::CH_NONE
					);

					if (itemToDraw == menuSelection)
					{
						if (menuSelection == Constants::LSMI_CANCEL)
						{
							DialogControls::DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
							DialogControls::DrawChevron(drawingSink, 195 + vignetteWidth + 7, y + 21, true, frame);
						}
						else
						{
							DialogControls::DrawChevron(drawingSink, 195 - 9, y + 105, true, frame);
							DialogControls::DrawChevron(drawingSink, 195 + vignetteWidth + 9, y + 105, false, frame);
						}
					}

					if (itemToDraw == Constants::LSMI_CANCEL)
					{
						y += backGap;
					}

					itemToDraw = static_cast<Constants::LangSettingsMenuItems>(itemToDraw + 1);
				}

				Drawing::EndRenderDrawing(renderer, drawingTexture, nullptr);
				DialogControls::DrawLabel(renderer, 235, startY + 0 * stride + 0 * backGap, cancelTexture, &cancelRect);

				/* render the carousel choice (and the sliding animation) */
				AnimateCarouselInternal();
				carouselSrcRect.x = sliderOffsetLeft;
				SDL_RenderTexture(renderer, slidingLangs, &carouselSrcRect, &carouselDestRect);
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		TeardownInternal();
		titleTexture&& [titleTexture] { SDL_DestroyTexture(titleTexture); return false; }();
		cancelTexture&& [cancelTexture] { SDL_DestroyTexture(cancelTexture); return false; }();

		return menuState.dialogResult;
	}

	void AnimateCarouselInternal()
	{
		if (movingToLanguage == selectedLanguage)
		{
			slideSpeed = 0;

			return;
		}

		if (slideSpeed == 0)
		{
			targetOffsetLeft = bounceMargin + static_cast<float>(movingToLanguage) * vignetteWidth;
			slideSpeed = 40;
		}

		const auto direction = targetOffsetLeft < sliderOffsetLeft ? -1 : 1;

		sliderOffsetLeft += static_cast<float>(direction) * slideSpeed;
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

	void PrepareControlsInternal(SDL_Renderer* renderer)
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
				slidingLangs = nullptr;

				return;
			}

			if (!SDL_SetRenderTarget(renderer, slidingLangs))
			{
				const auto targetError = SDL_GetError();

				ReportError("Could not set sliding texture as the render target", targetError);

				return;
			}

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG_FAT, 28, 0, 30, literalSettingsLanguageEN);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 13, 0, 70, literalSettingsLanguageENDetails);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 23, 0, 190, literalSettingsLanguageENName);

			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG_FAT, 28, 1, 30, literalSettingsLanguageFI);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 13, 1, 70, literalSettingsLanguageFIDetails);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 23, 1, 190, literalSettingsLanguageFIName);

			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG_FAT, 28, 2, 30, literalSettingsLanguageDE);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 13, 2, 70, literalSettingsLanguageDEDetails);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 23, 2, 190, literalSettingsLanguageDEName);

			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG_FAT, 28, 3, 30, literalSettingsLanguageUA);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 13, 3, 70, literalSettingsLanguageUADetails);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 23, 3, 190, literalSettingsLanguageUAName);

			MenuCommon::DrawIcon(renderer, CHUNK_KEY_LANG_FLAG_AM_PNG, 0, vignetteWidth, bounceMargin);
			MenuCommon::DrawIcon(renderer, CHUNK_KEY_LANG_FLAG_FI_PNG, 1, vignetteWidth, bounceMargin);
			MenuCommon::DrawIcon(renderer, CHUNK_KEY_LANG_FLAG_DE_PNG, 2, vignetteWidth, bounceMargin);
			MenuCommon::DrawIcon(renderer, CHUNK_KEY_LANG_FLAG_UA_PNG, 3, vignetteWidth, bounceMargin);

			if (!SDL_SetRenderTarget(renderer, nullptr))
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

	void VignetteLabelInternal(SDL_Renderer* renderer, const int font, const int size, const int vignetteIndex, const int y, const char* text)
	{
		SDL_FRect rectLabel = { 0,0,0,0 };
		const auto textureLabel = Scripture::RenderText(
			renderer,
			&rectLabel,
			font,
			size,
			text,
			{ 250, 230, 230, 245 }
		);

		DialogControls::CenterLabel(
			renderer,
			static_cast<int>(bounceMargin + static_cast<float>(vignetteIndex) * vignetteWidth + vignetteWidth / 2),
			y,
			textureLabel,
			&rectLabel
		);

		SDL_DestroyTexture(textureLabel);
	}

	void TeardownInternal()
	{
		Drawing::DestroyTexture(&slidingLangs);
	}
}