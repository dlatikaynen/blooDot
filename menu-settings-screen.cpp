#include "pch.h"
#include "menu-settings-screen.h"

#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo.h>
#include "xlations.h"
#include "settings.h"

extern SettingsStruct Settings;

SDL_Event screenSettingsMenuEvent;
bool screenSettingsMenuRunning = false;
ScreenSettingsMenuItems menuSelection = SSMI_CANCEL;

bool ScreenSettingsMenuLoop(SDL_Renderer* renderer)
{
	screenSettingsMenuRunning = true;

	SDL_Rect outerMenuRect{ 150,45,340,390 };
	SDL_Rect titleRect{ 0,0,0,0 };
	SDL_Rect cancelRect{ 0,0,0,0 };
	SDL_Rect hintRect{ 0,0,0,0 };

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
		{ 250, 230, 230, 245 }
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
				case SDL_SCANCODE_UP:
				case SDL_SCANCODE_KP_8:
				case SDL_SCANCODE_W:
					if (menuSelection != SSMI_CANCEL)
					{
						menuSelection = static_cast<ScreenSettingsMenuItems>(menuSelection - 1);
					}

					break;

				case SDL_SCANCODE_DOWN:
				case SDL_SCANCODE_KP_2:
				case SDL_SCANCODE_S:
					if (menuSelection != SSMI_VIDEOMODE_EGA)
					{
						menuSelection = static_cast<ScreenSettingsMenuItems>(menuSelection + 1);
					}

					break;

				case SDL_SCANCODE_PAGEDOWN:
				case SDL_SCANCODE_END:
					menuSelection = SSMI_VIDEOMODE_FULLSCREEN;
					break;

				case SDL_SCANCODE_PAGEUP:
				case SDL_SCANCODE_HOME:
					menuSelection = SSMI_VIDEOMODE_GOD;
					break;

				case SDL_SCANCODE_RETURN:
				case SDL_SCANCODE_RETURN2:
				case SDL_SCANCODE_KP_ENTER:
				case SDL_SCANCODE_SPACE:
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

		const auto drawingTexture = BeginRenderDrawing(renderer);
		if (drawingTexture) [[likely]]
		{
			const auto drawingSink = GetDrawingSink();
			const int stride = 46;
			const int backGap = stride / 2;
			ScreenSettingsMenuItems itemToDraw = SSMI_CANCEL;
			for (auto y = 94; y < 200; y += stride)
			{
				DrawButton(
					drawingSink,
					195,
					y,
					250,
					itemToDraw > SSMI_CANCEL ? 232 : 42,
					itemToDraw == menuSelection
				);

				if (itemToDraw == menuSelection)
				{
					if (menuSelection == SSMI_CANCEL)
					{
						DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
						DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);
					}
					else
					{
						DrawChevron(drawingSink, 195 - 9, y + 83, true, frame);
						DrawChevron(drawingSink, 195 + 250 + 9, y + 83, false, frame);
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
			/*
			DrawLabel(renderer, 235, 100 + 1 * stride + 1 * backGap, uuupTexture, &uuupRect);
			DrawLabel(renderer, 235, 100 + 2 * stride + 1 * backGap, downTexture, &downRect);
			*/
			DrawLabel(renderer, 235, 400, hintTexture, &hintRect);
		}

		SDL_RenderPresent(renderer);
		SDL_Delay(16);
		++frame;
	}

	return screenSettingsMenuRunning;
}
