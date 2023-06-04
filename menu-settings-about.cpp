#include "pch.h"
#include "menu-settings-about.h"
#include "menu-common.h"
#include "constants.h"
#include "scripture.h"
#include "xlations.h"
#include "dialogcontrols.h"

extern bool mainRunning;

namespace blooDot::MenuSettingsAbout
{
	MenuDialogInteraction menuState;
	bool menuRunning = false;

	SDL_Texture* backTexture = NULL;
	std::vector<SDL_Texture*> sectionTextures;
	std::vector<SDL_Texture*> chapterTextures;
	std::vector<SDL_Texture*> creditsTextures;

	SDL_Rect backRect{ 0,0,0,0 };
	std::vector<SDL_Rect> sectionRects;
	std::vector<SDL_Rect> chapterRects;
	std::vector<SDL_Rect> creditsRects;

	bool MenuLoop(SDL_Renderer* renderer)
	{
		menuRunning = true;
		menuState.itemCount = AboutMenuItems::HAM_BACK + 1;
		menuState.selectedItemIndex = AboutMenuItems::HAM_BACK;

		SDL_Rect outerMenuRect{ 150,45,340,390 };
		SDL_Rect titleRect{ 0,0,0,0 };

		const auto titleTexture = RenderText(
			renderer,
			&titleRect,
			FONT_KEY_ALIEN,
			26,
			literalliteralAlienAboutMenuLabel,
			AlienTextColor
		);

		_PrepareText(renderer);

		unsigned short frame = 0;
		while (menuRunning && mainRunning)
		{
			blooDot::MenuCommon::HandleMenu(&menuState);
			if (menuState.leaveDialog || menuState.enterMenuItem)
			{
				/* "back" is the only menu item */
				menuRunning = false;
			}

			if (menuState.leaveMain)
			{
				mainRunning = false;
			}

			if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = IMG_GetError();
				ReportError("Failed to clear the about menu screen", clearError);
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
			SDL_RenderFillRect(renderer, &outerMenuRect);
			SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
			if (SDL_RenderDrawRect(renderer, &outerMenuRect) < 0)
			{
				const auto drawRectError = SDL_GetError();
				ReportError("Failed to draw about menu panel border", drawRectError);
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
				AboutMenuItems itemToDraw = AboutMenuItems::HAM_BACK;
				for (auto y = 94; y < 95; y += stride)
				{
					const auto thisItem = itemToDraw == menuState.selectedItemIndex;
					DrawButton(drawingSink, 195, y, 250, 42, thisItem);
					if (thisItem)
					{
						DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
						DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);
					}

					if (itemToDraw == AboutMenuItems::HAM_BACK)
					{
						y += backGap;
					}

					itemToDraw = static_cast<AboutMenuItems>(itemToDraw + 1);
				}

				EndRenderDrawing(renderer, drawingTexture, nullptr);

				DrawLabel(renderer, 235, yStart + 0 * stride + 0 * backGap, backTexture, &backRect);

				/* the storyboarded "rolling credits */
				DrawLabel(renderer, 195, yStart + 1 * stride + 1 * backGap, sectionTextures[0], &sectionRects[0]);

			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		_Teardown();
		SDL_DestroyTexture(titleTexture);

		return menuRunning;
	}

	void _PrepareText(SDL_Renderer* renderer)
	{
		backTexture = RenderText(renderer, &backRect, FONT_KEY_DIALOG, 23, literalMenuBack, ButtonTextColor);
		
		SDL_Rect textureRect;
		const auto& texture001 = RenderText(renderer, &textureRect, FONT_KEY_DIALOG, 28, literalAboutClassificationProduction, ButtonTextColor, true);
		if (texture001)
		{
			sectionTextures.push_back(texture001);
			sectionRects.emplace_back(textureRect);
		}
	}

	void _Teardown()
	{
		backTexture&& [] { SDL_DestroyTexture(backTexture); return false; }();

		for (const auto& texture : creditsTextures)
		{
			if (texture)
			{
				SDL_DestroyTexture(texture);
			}
		}

		creditsTextures.clear();
		for (const auto& texture : chapterTextures)
		{
			if (texture)
			{
				SDL_DestroyTexture(texture);
			}
		}

		chapterTextures.clear();
		for (const auto& texture : sectionTextures)
		{
			if (texture)
			{
				SDL_DestroyTexture(texture);
			}
		}

		sectionTextures.clear();
	}
}