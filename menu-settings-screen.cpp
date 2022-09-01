#include "pch.h"
#include "menu-settings-screen.h"

#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo.h>
#include "xlations.h"
#include "settings.h"
#include <regex>
#include "constants.h"

constexpr int const bounceMargin = 10;
constexpr int const vignetteWidth = 250;
constexpr int const vignetteHeight = 220;
constexpr int const vignetteGap = 10;
constexpr int const vignetteCount = 6;

extern SettingsStruct Settings;

SDL_Event screenSettingsMenuEvent;
bool screenSettingsMenuRunning = false;
ScreenSettingsMenuItems menuSelection = SSMI_CANCEL;
ViewportResolutions selectedResolution = ViewportResolutions::VR_TEMPLE;
ViewportResolutions movingToResolution = ViewportResolutions::VR_TEMPLE;
SDL_Texture* slidingModes;
int sliderTextureWidth;
int sliderOffsetLeft = bounceMargin;
int slideSpeed = 0;
int targetOffsetLeft = 0;

bool ScreenSettingsMenuLoop(SDL_Renderer* renderer)
{
	constexpr int const startY = 94;
	constexpr int const stride = 46;
	constexpr int backGap = stride / 2;
	constexpr int const sliderY = startY + stride + backGap;
	constexpr SDL_Rect const carouselDestRect = { 195,sliderY,vignetteWidth,vignetteHeight };

	_PrepareControls(renderer);
	InitializeBoydsa(GodsPreferredWidth, GodsPreferredHight, 50, 370);
	screenSettingsMenuRunning = true;
	selectedResolution = Settings.SettingViewportResolution;
	movingToResolution = Settings.SettingViewportResolution;
	sliderOffsetLeft = static_cast<int>(selectedResolution) * vignetteWidth + bounceMargin;

	SDL_Rect outerMenuRect{ 150,45,340,390 };
	SDL_Rect titleRect{ 0,0,0,0 };
	SDL_Rect cancelRect{ 0,0,0,0 };
	SDL_Rect hintRect{ 0,0,0,0 };
	SDL_Rect carouselSrcRect = { sliderOffsetLeft,0,vignetteWidth,vignetteHeight };

	const auto titleTexture = RenderText(
		renderer,
		&titleRect,
		FONT_KEY_ALIEN,
		26,
		literalAlienScreenSettingsMenuLabel,
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

	/* explanation of why the settings window itself does not change */
	const auto hintTexture = RenderText(
		renderer,
		&hintRect,
		FONT_KEY_TITLE,
		13,
		literalSettingsMenuScreensizeHint,
		DialogTextColor
	);

	const auto hintShadow = RenderText(
		renderer,
		&hintRect,
		FONT_KEY_TITLE,
		13,
		literalSettingsMenuScreensizeHint,
		DialogTextShadowColor
	);

	unsigned short frame = 0L;
	while (screenSettingsMenuRunning)
	{
		while (SDL_PollEvent(&screenSettingsMenuEvent) != 0)
		{
			switch (screenSettingsMenuEvent.type)
			{
			case SDL_KEYDOWN:
				switch (screenSettingsMenuEvent.key.keysym.scancode)
				{
				case SDL_SCANCODE_DOWN:
				case SDL_SCANCODE_KP_2:
				case SDL_SCANCODE_S:
				case SDL_SCANCODE_PAGEDOWN:
				case SDL_SCANCODE_END:
					menuSelection = SSMI_VIDEOMODE;
					break;

				case SDL_SCANCODE_UP:
				case SDL_SCANCODE_KP_8:
				case SDL_SCANCODE_W:
				case SDL_SCANCODE_PAGEUP:
				case SDL_SCANCODE_HOME:
					menuSelection = SSMI_CANCEL;
					break;

				case SDL_SCANCODE_LEFT:
				case SDL_SCANCODE_KP_4:
				case SDL_SCANCODE_A:
					if (menuSelection == SSMI_CANCEL)
					{
						// TODO: soundeffect_moveinmenu_selecteditemchange
						menuSelection = SSMI_VIDEOMODE;
					}
					else if (selectedResolution == ViewportResolutions::VR_TEMPLE)
					{
						// TODO: soundeffect_moveinmenu_bounce
					}
					else if (movingToResolution == selectedResolution)
					{
						// TODO: soundeffect_moveinmenu_slide
						movingToResolution = static_cast<ViewportResolutions>(static_cast<int>(selectedResolution) - 1);
					}
					else if (movingToResolution != ViewportResolutions::VR_TEMPLE)
					{
						--movingToResolution;
					}

					break;

				case SDL_SCANCODE_RIGHT:
				case SDL_SCANCODE_KP_6:
				case SDL_SCANCODE_D:
					if (menuSelection == SSMI_CANCEL)
					{
						// TODO: soundeffect_moveinmenu_selecteditemchange
						menuSelection = SSMI_VIDEOMODE;
					}
					else if (selectedResolution == ViewportResolutions::VR_MAXOUT)
					{
						// TODO: soundeffect_moveinmenu_bounce
					}
					else if(movingToResolution == selectedResolution)
					{
						// TODO: soundeffect_moveinmenu_slide
						movingToResolution = static_cast<ViewportResolutions>(static_cast<int>(selectedResolution) + 1);
					}
					else if (movingToResolution != ViewportResolutions::VR_MAXOUT)
					{
						++movingToResolution;
					}

					break;

				case SDL_SCANCODE_RETURN:
				case SDL_SCANCODE_RETURN2:
				case SDL_SCANCODE_KP_ENTER:
				case SDL_SCANCODE_SPACE:
					if (menuSelection == SSMI_VIDEOMODE && _CanSelectMode(renderer))
					{
						Settings.SettingViewportResolution = movingToResolution;
					}

					screenSettingsMenuRunning = false;
					break;

				case SDL_SCANCODE_ESCAPE:
					screenSettingsMenuRunning = false;
					break;
				}

				break;
			}
		}

		if (SDL_RenderClear(renderer) < 0)
		{
			const auto clearError = IMG_GetError();
			ReportError("Failed to clear the screen settings menu screen", clearError);
		}

		SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
		SDL_RenderFillRect(renderer, &outerMenuRect);
		SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
		if (SDL_RenderDrawRect(renderer, &outerMenuRect) < 0)
		{
			const auto drawRectError = SDL_GetError();
			ReportError("Failed to draw screen settings menu panel border", drawRectError);
			screenSettingsMenuRunning = false;
		};

		DrawLabel(renderer, 286, 54, titleTexture, &titleRect);

		const auto drawingTexture = BeginRenderDrawing(renderer, 640, 480);
		if (drawingTexture) [[likely]]
		{
			auto const& drawingSink = GetDrawingSink();

			IterateBoydsa();
			RenderBoydsa(drawingSink);

			ScreenSettingsMenuItems itemToDraw = SSMI_CANCEL;
			for (auto y = startY; y < 200; y += stride)
			{
				DrawButton(
					drawingSink,
					195,
					y,
					vignetteWidth,
					itemToDraw > SSMI_CANCEL ? vignetteHeight : 42,
					itemToDraw == menuSelection
				);

				if (itemToDraw == menuSelection)
				{
					if (menuSelection == SSMI_CANCEL)
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

				if (itemToDraw == SSMI_CANCEL)
				{
					y += backGap;
				}

				itemToDraw = static_cast<ScreenSettingsMenuItems>(itemToDraw + 1);
			}

			EndRenderDrawing(renderer, drawingTexture);

			DrawLabel(renderer, 235, 100 + 0 * stride + 0 * backGap, cancelTexture, &cancelRect);
			DrawLabel(renderer, 181, 392, hintShadow, &hintRect);
			DrawLabel(renderer, 180, 391, hintTexture, &hintRect);

			/* render the carousel choice (and the sliding animation) */
			_AnimateCarousel();
			carouselSrcRect.x = sliderOffsetLeft;
			SDL_RenderCopy(renderer, slidingModes, &carouselSrcRect, &carouselDestRect);
		}

		SDL_RenderPresent(renderer);
		//SDL_Delay(16);
		++frame;
	}

	TeardownBoydsa();
	_TeardownScreenSettingsMenu();
	titleTexture && [titleTexture] { SDL_DestroyTexture(titleTexture); return false; }();
	cancelTexture&& [cancelTexture] { SDL_DestroyTexture(cancelTexture); return false; }();
	hintTexture&& [hintTexture] { SDL_DestroyTexture(hintTexture); return false; }();
	hintShadow&& [hintShadow] { SDL_DestroyTexture(hintShadow); return false; }();

	return screenSettingsMenuRunning;
}

void _AnimateCarousel()
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

	auto direction = targetOffsetLeft < sliderOffsetLeft ? -1 : 1;
	sliderOffsetLeft += direction * slideSpeed;
	if (direction == 1)
	{
		if (sliderOffsetLeft >= targetOffsetLeft)
		{
			sliderOffsetLeft = targetOffsetLeft;
			selectedResolution = movingToResolution;
		}
	}
	else 
	{
		if (sliderOffsetLeft <= targetOffsetLeft)
		{
			sliderOffsetLeft = targetOffsetLeft;
			selectedResolution = movingToResolution;
		}
	}
}

void _PrepareControls(SDL_Renderer* renderer)
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
			slidingModes = NULL;

			return;
		}

		if (SDL_SetRenderTarget(renderer, slidingModes) < 0)
		{
			const auto targetError = SDL_GetError();
			ReportError("Could not set sliding texture as the render target", targetError);
			return;
		}

		_VignetteLabel(renderer, FONT_KEY_DIALOG, 28, 0, 30, literalSettingsScreenTemple);
		_VignetteLabel(renderer, FONT_KEY_TITLE, 13, 0, 70, literalSettingsScreenTempleDetails);
		_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 0, 190, literalSettingsScreenTempleResolution);

		_VignetteLabel(renderer, FONT_KEY_DIALOG, 28, 1, 30, literalSettingsScreenHercules);
		_VignetteLabel(renderer, FONT_KEY_TITLE, 13, 1, 70, literalSettingsScreenHerculesDetails);
		_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 1, 190, literalSettingsScreenHerculesResolution);

		_VignetteLabel(renderer, FONT_KEY_DIALOG, 28, 2, 30, literalSettingsScreenModeX);
		_VignetteLabel(renderer, FONT_KEY_TITLE, 13, 2, 70, literalSettingsScreenModexDetails);
		_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 2, 190, literalSettingsScreenModeXResolution);

		_VignetteLabel(renderer, FONT_KEY_DIALOG, 28, 3, 30, literalSettingsScreenSVGA);
		_VignetteLabel(renderer, FONT_KEY_TITLE, 13, 3, 70, literalSettingsScreenSVGADetails);
		_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 3, 190, literalSettingsScreenSVGAResolution);

		_VignetteLabel(renderer, FONT_KEY_DIALOG, 28, 4, 30, literalSettingsScreenNotebook);
		_VignetteLabel(renderer, FONT_KEY_TITLE, 13, 4, 70, literalSettingsScreenNotebookDetails);
		_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 4, 190, literalSettingsScreenNotebookResolution);

		_VignetteLabel(renderer, FONT_KEY_DIALOG, 28, 5, 30, literalSettingsScreenFull);
		_VignetteLabel(renderer, FONT_KEY_TITLE, 13, 5, 70, literalSettingsScreenFullDetails);

		SDL_Rect screen;
		if(_GetResolution(VR_MAXOUT, &screen))
		{
			std::stringstream screenWidth;
			std::stringstream screenHeight;
			screenWidth << screen.w;
			screenHeight << screen.h;
			std::string fullScreenDimsTemplate;
			fullScreenDimsTemplate.assign(literalSettingsScreenFullResolution);
			auto fullScreenDims1 = std::regex_replace(fullScreenDimsTemplate, std::regex("\\$w"), screenWidth.str());
			auto fullScreenDims2 = std::regex_replace(fullScreenDims1, std::regex("\\$h"), screenHeight.str());
			_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 5, 190, fullScreenDims2.c_str());
		}
		
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

void _TeardownScreenSettingsMenu()
{
	slidingModes&& [] { SDL_DestroyTexture(slidingModes); return false; }();
}

bool _CanSelectMode(SDL_Renderer* renderer)
{
	if (selectedResolution != VR_MAXOUT)
	{
		/* we won't precompute this as they could move
		 * the launcher to a bigger screen, or change settings while we run */
		SDL_Rect screen;
		_GetResolution(VR_MAXOUT, &screen);

		SDL_Rect desired;
		_GetResolution(selectedResolution, &desired);

		if (desired.w > screen.w || desired.h > screen.h)
		{
			GpuChanLoop(
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

bool _GetResolution(ViewportResolutions videoMode, SDL_Rect* dimensions)
{
	auto& rect = (*dimensions);
	SDL_DisplayMode displayMode = {};

	switch (videoMode)
	{
	case ViewportResolutions::VR_HERCMONO:
		rect.w = 720;
		rect.h = 320;
		break;

	case ViewportResolutions::VR_TEMPLE:
		rect.w = GodsPreferredWidth;
		rect.h = GodsPreferredHight;
		break;

	case ViewportResolutions::VR_MODEX:
		rect.w = 320;
		rect.h = 240;
		break;

	case ViewportResolutions::VR_SVGA:
		rect.w = 800;
		rect.h = 600;
		break;

	case ViewportResolutions::VR_NOTEBOOK:
		rect.w = 1280;
		rect.h = 768;
		break;

	case ViewportResolutions::VR_MAXOUT:
		if (SDL_GetDesktopDisplayMode(0, &displayMode) < 0)
		{
			const auto& screenError = SDL_GetError();
			ReportError("Could not query screen size", screenError);
			return false;
		}

		rect.w = displayMode.w;
		rect.h = displayMode.h;
		break;

	default:
		rect.w = 0;
		rect.h = 0;
		break;
	}

	return true;
}