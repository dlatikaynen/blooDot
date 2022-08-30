#include "pch.h"
#include "menu-settings-screen.h"

#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo.h>
#include "xlations.h"
#include "settings.h"

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
	InitializeBoydsa(640, 480, 50, 370);
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
		{ 250,200,200,222 }
	);

	/* menu item text */
	const auto cancelTexture = RenderText(
		renderer,
		&cancelRect,
		FONT_KEY_DIALOG,
		23,
		literalMenuCancel,
		{ 250, 230, 230, 245 }
	);

	/* explanation of why the settings window itself does not change */
	const auto hintTexture = RenderText(
		renderer,
		&hintRect,
		FONT_KEY_TITLE,
		13,
		literalSettingsMenuScreensizeHint,
		{ 250, 250, 250, 255 }
	);

	const auto hintShadow = RenderText(
		renderer,
		&hintRect,
		FONT_KEY_TITLE,
		13,
		literalSettingsMenuScreensizeHint,
		{ 65, 56, 56, 200 }
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
					Settings.SettingViewportResolution = movingToResolution;
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
		targetOffsetLeft = static_cast<int>(movingToResolution) * vignetteWidth + bounceMargin;
		slideSpeed = 40;
	}

	auto direction = targetOffsetLeft < sliderOffsetLeft ? -1 : 1;
	sliderOffsetLeft += direction * slideSpeed;
	if (direction == 1)
	{
		if (sliderOffsetLeft >= targetOffsetLeft)
		{
			selectedResolution = movingToResolution;
		}
	}
	else 
	{
		if (sliderOffsetLeft <= targetOffsetLeft)
		{
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

		SDL_Rect godRect = { 0,0,0,0 };
		const auto godTexture = RenderText(
			renderer,
			&godRect,
			FONT_KEY_DIALOG,
			23,
			literalSettingsScreenTempleResolution,
			{ 250, 230, 230, 245 }
		);

		auto const& drawingTexture = BeginRenderDrawing(renderer, sliderTextureWidth, vignetteHeight);
		auto const& drawingSink = GetDrawingSink();
		cairo_set_source_rgba(drawingSink, 1, .5, .9, .73);
		cairo_rectangle(drawingSink, 30, 50, vignetteWidth - 60, vignetteHeight - 60);
		cairo_fill(drawingSink);
		
		cairo_set_source_rgba(drawingSink, 1, 1, 1, 1);
		cairo_move_to(drawingSink, 0, 0);
		cairo_line_to(drawingSink, sliderTextureWidth, vignetteHeight);
		cairo_move_to(drawingSink, sliderTextureWidth, 0);
		cairo_line_to(drawingSink, 0, vignetteHeight);
		cairo_stroke(drawingSink);
		
		EndRenderDrawing(renderer, drawingTexture);

		DrawLabel(renderer, 30, 100, godTexture, &godRect);

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

void _TeardownScreenSettingsMenu()
{
	slidingModes&& [] { SDL_DestroyTexture(slidingModes); return false; }();
}