#include "menu-settings-displays.h"

#include <regex>
#include <sstream>
#include <string>

#include "gpuchan.h"
#include "dialog-controls.h"
#include "drawing.h"
#include "scripture.h"
#include "menu-common.h"
#include "layout-constants.h"
#include "../../res/xlations.h"
#include "../../res/chunk-constants.h"
#include "../../main.h"
#include "../../src/snd/sfx.h"
#include "../state/settings.h"
#include "../algo/boydsa.h"

namespace blooDot::MenuSettingsDisplays
{
	constexpr int bounceMargin = 10;
	constexpr int vignetteWidth = 250;
	constexpr int vignetteHeight = 220;
	constexpr int vignetteGap = 10;
	constexpr int vignetteCount = 7;

	MenuCommon::MenuDialogInteraction menuState;
	SDL_Event screenSettingsMenuEvent;
	auto menuSelection = Constants::ScreenSettingsMenuItems::SSMI_CANCEL;
	auto selectedResolution = Constants::ViewportResolutions::VR_TEMPLE;
	auto movingToResolution = Constants::ViewportResolutions::VR_TEMPLE;
	SDL_Texture* slidingModes;
	int sliderTextureWidth;
	float sliderOffsetLeft = bounceMargin;
	int slideSpeed = 0;
	int targetOffsetLeft = 0;

	bool ScreenSettingsMenuLoop(SDL_Renderer* renderer)
	{
		constexpr int startY = 94;
		constexpr int stride = 46;
		constexpr int labelInsetX = 45;
		constexpr int backGap = stride / 2;
		constexpr int sliderY = startY + stride + backGap;

		constexpr SDL_FRect outerMenuRect{ 150,45,340,390 };
		constexpr SDL_FRect carouselDestRect = { outerMenuRect.x + labelInsetX,sliderY,vignetteWidth,vignetteHeight };

		menuState.leaveDialog = false;
		menuState.dialogResult = Constants::DMR_CANCEL;

		PrepareControlsInternal(renderer);
		Algo::Boyds::InitializeBoydsa(Constants::GodsPreferredWidth, Constants::GodsPreferredHight, 50, 370);
		selectedResolution = Settings::SettingsData.SettingViewportResolution;
		movingToResolution = Settings::SettingsData.SettingViewportResolution;
		sliderOffsetLeft = static_cast<float>(selectedResolution) * vignetteWidth + bounceMargin;

		SDL_FRect titleRect{ 0,0,0,0 };
		SDL_FRect cancelRect{ 0,0,0,0 };
		SDL_FRect hintRect{ 0,0,0,0 };
		SDL_FRect carouselSrcRect = { sliderOffsetLeft,0,vignetteWidth,vignetteHeight };

		const auto titleTexture = Scripture::RenderText(
			renderer,
			&titleRect,
			Scripture::FONT_KEY_ALIEN,
			Ui::MenuPanelTitleFontSize,
			literalAlienScreenSettingsMenuLabel,
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

		/* explanation of why the settings window itself does not change */
		const auto hintTexture = Scripture::RenderText(
			renderer,
			&hintRect,
			Scripture::FONT_KEY_DIALOG,
			Ui::AnnotationLabelFontSize,
			literalSettingsMenuScreensizeHint,
			Constants::DialogTextColor
		);

		const auto hintShadow = Scripture::RenderText(
			renderer,
			&hintRect,
			Scripture::FONT_KEY_DIALOG,
			Ui::AnnotationLabelFontSize,
			literalSettingsMenuScreensizeHint,
			Constants::DialogTextShadowColor
		);

		unsigned short frame = 0L;
		while (!menuState.leaveDialog)
		{
			while (SDL_PollEvent(&screenSettingsMenuEvent) != 0)
			{
				switch (screenSettingsMenuEvent.type)
				{
				case SDL_EVENT_QUIT:
					menuState.leaveMain = true;
					menuState.leaveDialog = true;

					break;

				case SDL_EVENT_KEY_DOWN:
					switch (screenSettingsMenuEvent.key.scancode)
					{
					case SDL_SCANCODE_DOWN:
					case SDL_SCANCODE_KP_2:
					case SDL_SCANCODE_S:
					case SDL_SCANCODE_PAGEDOWN:
					case SDL_SCANCODE_END:
						if (menuSelection == Constants::SSMI_VIDEOMODE)
						{
							Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = Constants::SSMI_VIDEOMODE;
							Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_UP:
					case SDL_SCANCODE_KP_8:
					case SDL_SCANCODE_W:
					case SDL_SCANCODE_PAGEUP:
					case SDL_SCANCODE_HOME:
						if (menuSelection == Constants::SSMI_CANCEL)
						{
							Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = Constants::SSMI_CANCEL;
							Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_LEFT:
					case SDL_SCANCODE_KP_4:
					case SDL_SCANCODE_A:
						if (menuSelection == Constants::SSMI_CANCEL)
						{
							menuSelection = Constants::SSMI_VIDEOMODE;
						}

						if (selectedResolution == Constants::ViewportResolutions::VR_TEMPLE)
						{
							Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
						}
						else if (movingToResolution == selectedResolution)
						{
							movingToResolution = static_cast<Constants::ViewportResolutions>(static_cast<int>(selectedResolution) - 1);
							Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
						}
						else if (movingToResolution != Constants::ViewportResolutions::VR_TEMPLE)
						{
							--movingToResolution;
						}

						break;

					case SDL_SCANCODE_RIGHT:
					case SDL_SCANCODE_KP_6:
					case SDL_SCANCODE_D:
						if (menuSelection == Constants::SSMI_CANCEL)
						{
							menuSelection = Constants::SSMI_VIDEOMODE;
						}

						if (selectedResolution == Constants::ViewportResolutions::VR_MAXOUT)
						{
							Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
						}
						else if (movingToResolution == selectedResolution)
						{
							movingToResolution = static_cast<Constants::ViewportResolutions>(static_cast<int>(selectedResolution) + 1);
							Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
						}
						else if (movingToResolution != Constants::ViewportResolutions::VR_MAXOUT)
						{
							++movingToResolution;
						}

						break;

					case SDL_SCANCODE_RETURN:
					case SDL_SCANCODE_RETURN2:
					case SDL_SCANCODE_KP_ENTER:
					case SDL_SCANCODE_SPACE:
						if (menuSelection == Constants::SSMI_VIDEOMODE && CanSelectModeInternal(renderer))
						{
							Settings::SettingsData.SettingViewportResolution = movingToResolution;
							menuState.leaveDialog = true;
						}
						else if (menuSelection == Constants::SSMI_CANCEL)
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

				ReportError("Failed to clear the screen settings menu screen", clearError);
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
			SDL_RenderFillRect(renderer, &outerMenuRect);
			SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
			if (!SDL_RenderRect(renderer, &outerMenuRect))
			{
				const auto drawRectError = SDL_GetError();

				ReportError("Failed to draw screen settings menu panel border", drawRectError);
				menuState.leaveDialog = true;
			};

			DialogControls::DrawLabel(renderer, 286, 51, titleTexture, &titleRect);

			if (const auto drawingTexture = Drawing::BeginRenderDrawing(renderer, 640, 480)) [[likely]]
			{
				auto const& drawingSink = Drawing::GetDrawingSink();

				Algo::Boyds::IterateBoydsa();
				Algo::Boyds::RenderBoydsa(drawingSink);

				Constants::ScreenSettingsMenuItems itemToDraw = Constants::SSMI_CANCEL;
				constexpr int buttonLeft = outerMenuRect.x + labelInsetX;

				for (auto y = startY; y < 200; y += stride)
				{
					DialogControls::DrawButton(
						drawingSink,
						buttonLeft,
						y,
						vignetteWidth,
						itemToDraw > Constants::SSMI_CANCEL ? vignetteHeight : 42,
						itemToDraw == menuSelection ? Constants::CH_MAINMENU : Constants::CH_NONE
					);

					if (itemToDraw == menuSelection)
					{
						if (menuSelection == Constants::SSMI_CANCEL)
						{
							DialogControls::DrawChevron(drawingSink, buttonLeft - 7, y + 21, false, frame);
							DialogControls::DrawChevron(drawingSink, buttonLeft + vignetteWidth + 7, y + 21, true, frame);
						}
						else
						{
							DialogControls::DrawChevron(drawingSink, buttonLeft - 9, y + 105, true, frame);
							DialogControls::DrawChevron(drawingSink, buttonLeft + vignetteWidth + 9, y + 105, false, frame);
						}
					}

					if (itemToDraw == Constants::SSMI_CANCEL)
					{
						y += backGap;
					}

					itemToDraw = static_cast<Constants::ScreenSettingsMenuItems>(itemToDraw + 1);
				}

				Drawing::EndRenderDrawing(renderer, drawingTexture, nullptr);

				DialogControls::DrawLabel(renderer, 235, startY + 0 * stride + 0 * backGap, cancelTexture, &cancelRect);
				DialogControls::DrawLabel(renderer, 181, 389, hintShadow, &hintRect);
				DialogControls::DrawLabel(renderer, 180, 388, hintTexture, &hintRect);

				/* render the carousel choice (and the sliding animation) */
				AnimateCarouselInternal();
				carouselSrcRect.x = sliderOffsetLeft;
				SDL_RenderTexture(renderer, slidingModes, &carouselSrcRect, &carouselDestRect);
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		Algo::Boyds::TeardownBoydsa();
		TeardownInternal();
		titleTexture&& [titleTexture] { SDL_DestroyTexture(titleTexture); return false; }();
		cancelTexture&& [cancelTexture] { SDL_DestroyTexture(cancelTexture); return false; }();
		hintTexture&& [hintTexture] { SDL_DestroyTexture(hintTexture); return false; }();
		hintShadow&& [hintShadow] { SDL_DestroyTexture(hintShadow); return false; }();

		return !menuState.leaveMain;
	}

	void AnimateCarouselInternal()
	{
		if (movingToResolution == selectedResolution)
		{
			slideSpeed = 0;

			return;
		}

		if (slideSpeed == 0)
		{
			targetOffsetLeft = bounceMargin + static_cast<int>(movingToResolution) * vignetteWidth;
			slideSpeed = 40;
		}

		const auto direction = static_cast<float>(targetOffsetLeft) < sliderOffsetLeft ? -1 : 1;

		sliderOffsetLeft += static_cast<float>(direction * slideSpeed);
		if (direction == 1)
		{
			if (sliderOffsetLeft >= static_cast<float>(targetOffsetLeft))
			{
				sliderOffsetLeft = static_cast<float>(targetOffsetLeft);
				selectedResolution = movingToResolution;
			}
		}
		else
		{
			if (sliderOffsetLeft <= static_cast<float>(targetOffsetLeft))
			{
				sliderOffsetLeft = static_cast<float>(targetOffsetLeft);
				selectedResolution = movingToResolution;
			}
		}
	}

	void PrepareControlsInternal(SDL_Renderer* renderer)
	{
		sliderTextureWidth = vignetteCount * vignetteWidth + (vignetteCount - 1) * vignetteGap + 2 * bounceMargin;
		slidingModes = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET,
			sliderTextureWidth,
			vignetteHeight
		);

		if (slidingModes)
		{
			if (SDL_SetTextureBlendMode(slidingModes, SDL_BLENDMODE_BLEND) < 0)
			{
				const auto carouselBlendmodeError = SDL_GetError();
				ReportError("Could not set blend mode of sliding texture", carouselBlendmodeError);
				SDL_DestroyTexture(slidingModes);
				slidingModes = nullptr;

				return;
			}

			if (SDL_SetRenderTarget(renderer, slidingModes) < 0)
			{
				const auto targetError = SDL_GetError();

				ReportError("Could not set sliding texture as the render target", targetError);

				return;
			}

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 28, 0, 30, literalSettingsScreenTemple);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG_FAT, 13, 0, 70, literalSettingsScreenTempleDetails);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 23, 0, 190, literalSettingsScreenTempleResolution);

			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 28, 1, 30, literalSettingsScreenHercules);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG_FAT, 13, 1, 70, literalSettingsScreenHerculesDetails);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 23, 1, 190, literalSettingsScreenHerculesResolution);

			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 28, 2, 30, literalSettingsScreenModeX);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG_FAT, 13, 2, 70, literalSettingsScreenModexDetails);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 23, 2, 190, literalSettingsScreenModeXResolution);

			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 28, 3, 30, literalSettingsScreenSVGA);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG_FAT, 13, 3, 70, literalSettingsScreenSVGADetails);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 23, 3, 190, literalSettingsScreenSVGAResolution);

			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 28, 4, 30, literalSettingsScreenNotebook);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG_FAT, 13, 4, 70, literalSettingsScreenNotebookDetails);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 23, 4, 190, literalSettingsScreenNotebookResolution);

			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 28, 5, 30, literalSettingsScreenSquare);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG_FAT, 13, 5, 70, literalSettingsScreenSquareDetails);

			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 28, 6, 30, literalSettingsScreenFull);
			VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG_FAT, 13, 6, 70, literalSettingsScreenFullDetails);

			SDL_Rect screen;
			if (MenuSettingsDisplays::GetResolutionInternal(Constants::VR_MAXOUT, &screen))
			{
				std::stringstream screenWidth;
				std::stringstream screenHeight;
				screenWidth << screen.w;
				screenHeight << screen.h;
				std::string fullScreenDimsTemplate;
				std::string squareDimsTemplate;
				fullScreenDimsTemplate.assign(literalSettingsScreenFullResolution);
				squareDimsTemplate.assign(literalSettingsScreenSquareResolution);

				auto fullScreenDims1 = std::regex_replace(fullScreenDimsTemplate, std::regex("\\$w"), screenWidth.str());
				auto fullScreenDims2 = std::regex_replace(fullScreenDims1, std::regex("\\$h"), screenHeight.str());
				auto fullScreenDims3 = std::regex_replace(squareDimsTemplate, std::regex("\\$h"), screenHeight.str());

				VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 23, 5, 190, fullScreenDims3.c_str());
				VignetteLabelInternal(renderer, Scripture::FONT_KEY_DIALOG, 23, 6, 190, fullScreenDims2.c_str());
			}

			MenuCommon::DrawIcon(renderer, CHUNK_KEY_UI_ICON_DIVINE, 0, vignetteWidth, bounceMargin);
			MenuCommon::DrawIcon(renderer, CHUNK_KEY_UI_ICON_HERC, 1, vignetteWidth, bounceMargin);
			MenuCommon::DrawIcon(renderer, CHUNK_KEY_UI_ICON_MODEX, 2, vignetteWidth, bounceMargin);
			MenuCommon::DrawIcon(renderer, CHUNK_KEY_UI_ICON_HD, 4, vignetteWidth, bounceMargin);
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
			bounceMargin + vignetteIndex * vignetteWidth + vignetteWidth / 2,
			y,
			textureLabel,
			&rectLabel
		);

		SDL_DestroyTexture(textureLabel);
	}

	void TeardownInternal()
	{
		Drawing::DestroyTexture(&slidingModes);
	}

	bool CanSelectModeInternal(SDL_Renderer* renderer)
	{
		if (selectedResolution != Constants::VR_MAXOUT)
		{
			/* we won't precompute this as they could move
			 * the launcher to a bigger screen, or change settings while we run */
			SDL_Rect screen;
			MenuSettingsDisplays::GetResolutionInternal(Constants::VR_MAXOUT, &screen);

			SDL_Rect desired;
			MenuSettingsDisplays::GetResolutionInternal(selectedResolution, &desired);

			if (desired.w > screen.w || desired.h > screen.h)
			{
				// does not fit
				Ui::GpuChan::GpuChanLoop(
					renderer,
					literalMessageInsufficientScreenSize,
					literalDialogTitleGpuChan,
					literalDialogBubbleGpuChan
				);

				return false;
			}
		}

		return true;
	}

	bool GetResolutionInternal(Constants::ViewportResolutions videoMode, SDL_Rect* dimensions)
	{
		auto& rect = (*dimensions);

		switch (videoMode)
		{
		case Constants::ViewportResolutions::VR_HERCMONO:
			rect.w = 720;
			rect.h = 320;
			break;

		case Constants::ViewportResolutions::VR_TEMPLE:
			rect.w = Constants::GodsPreferredWidth;
			rect.h = Constants::GodsPreferredHight;
			break;

		case Constants::ViewportResolutions::VR_MODEX:
			rect.w = 320;
			rect.h = 240;
			break;

		case Constants::ViewportResolutions::VR_SVGA:
			rect.w = 800;
			rect.h = 600;
			break;

		case Constants::ViewportResolutions::VR_NOTEBOOK:
			rect.w = 1280;
			rect.h = 768;
			break;

		case Constants::ViewportResolutions::VR_SQUARE: {
			const auto& displayMode = SDL_GetDesktopDisplayMode(0);

			if (displayMode == nullptr)
			{
				const auto& screenError = SDL_GetError();

				ReportError("Could not query screen size", screenError);

				return false;
			}

			rect.w = std::min(displayMode->w, displayMode->h);
			rect.h = std::min(displayMode->h, displayMode->w);

			break;
		}

		case Constants::ViewportResolutions::VR_MAXOUT: {
			const auto& displayMode = SDL_GetDesktopDisplayMode(0);

			if (displayMode == nullptr)
			{
				const auto& screenError = SDL_GetError();

				ReportError("Could not query screen size", screenError);

				return false;
			}

			rect.w = displayMode->w;
			rect.h = displayMode->h;

			break;
		}

		default:
			rect.w = 0;
			rect.h = 0;

			break;
		}

		return true;
	}
}