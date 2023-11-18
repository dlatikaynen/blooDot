#include <cairo/cairo.h>
#include "pch.h"
#include "menu-creatormode.h"
#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include "xlations.h"
#include "constants.h"
#include "sfx.h"
#include "menu-common.h"
#include "makersettings.h"

extern bool mainRunning;

namespace blooDot::MenuCreatorMode
{
	MenuDialogInteraction menuState;
	bool menuRunning = false;

	SDL_Texture* backTexture = NULL;
	SDL_Texture* lastLocationTexture = NULL;
	SDL_Texture* teleportTexture = NULL;

	SDL_Rect backRect{ 0,0,0,0 };
	SDL_Rect lastLocationRect{ 0,0,0,0 };
	SDL_Rect teleportRect{ 0,0,0,0 };

	bool MenuLoop(SDL_Renderer* renderer)
	{
		menuRunning = true;
		menuState.itemCount = CreatorModeMenuItems::CMMI_TELEPORT + 1;
		menuState.selectedItemIndex = CreatorModeMenuItems::CMMI_BACK;

		SDL_Rect outerMenuRect{ 150,45,340,390 };
		SDL_Rect titleRect{ 0,0,0,0 };

		const auto titleTexture = RenderText(
			renderer,
			&titleRect,
			FONT_KEY_ALIEN,
			26,
			literalAlienMakerMenuLabel,
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
				menuRunning = _EnterAndHandleCreatorModeMenuItem(renderer);
			}

			if (menuState.leaveMain)
			{
				mainRunning = false;
			}

			if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = IMG_GetError();
				ReportError("Failed to clear the creator mode menu screen", clearError);
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
			SDL_RenderFillRect(renderer, &outerMenuRect);
			SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
			if (SDL_RenderDrawRect(renderer, &outerMenuRect) < 0)
			{
				const auto drawRectError = SDL_GetError();
				ReportError("Failed to draw creator mode menu panel border", drawRectError);
				menuRunning = false;
			};

			DrawLabel(renderer, 286, 51, titleTexture, &titleRect);

			const auto drawingTexture = BeginRenderDrawing(renderer, GodsPreferredWidth, GodsPreferredHight);
			if (drawingTexture)
			{
				const auto drawingSink = GetDrawingSink();
				const int yStart = 94;
				const int stride = 46;
				const int backGap = stride / 2;
				const int yEnd = yStart + backGap + menuState.itemCount * stride;
				CreatorModeMenuItems itemToDraw = CMMI_BACK;
				for (auto y = 94; y < yEnd; y += stride)
				{
					const auto thisItem = itemToDraw == menuState.selectedItemIndex;
					DrawButton(drawingSink, 195, y, 250, 42, thisItem);
					if (thisItem)
					{
						DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
						DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);
					}

					if (itemToDraw == CreatorModeMenuItems::CMMI_BACK)
					{
						y += backGap;
					}

					itemToDraw = static_cast<CreatorModeMenuItems>(itemToDraw + 1);
				}

				EndRenderDrawing(renderer, drawingTexture, nullptr);

				DrawLabel(renderer, 235, yStart + 0 * stride + 0 * backGap, backTexture, &backRect);
				DrawLabel(renderer, 235, yStart + 1 * stride + 1 * backGap, lastLocationTexture, &lastLocationRect);
				DrawLabel(renderer, 235, yStart + 2 * stride + 1 * backGap, teleportTexture, &teleportRect);
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		SDL_DestroyTexture(titleTexture);
		_PrepareText(renderer, true);

		return menuRunning;
	}

	void _PrepareText(SDL_Renderer* renderer, bool destroy)
	{
		DestroyTexture(&backTexture);
		DestroyTexture(&lastLocationTexture);
		DestroyTexture(&teleportTexture);
		if (!destroy)
		{
			auto& lastLocationLabel = blooDot::MakerSettings::HasLastLocation()
				? literalMakerMenuLast
				: literalMakerMenuInitial;

			backTexture = RenderText(renderer, &backRect, FONT_KEY_DIALOG, 23, literalMenuBack, ButtonTextColor);
			lastLocationTexture = RenderText(renderer, &lastLocationRect, FONT_KEY_DIALOG, 23, lastLocationLabel, ButtonTextColor);
			teleportTexture = RenderText(renderer, &teleportRect, FONT_KEY_DIALOG, 23, literalMakerMenuTeleport, ButtonTextColor);
		}
	}

	bool _EnterAndHandleCreatorModeMenuItem(SDL_Renderer* renderer)
	{
		switch (menuState.selectedItemIndex)
		{
		case CreatorModeMenuItems::CMMI_LAST_LOCATION:
			break;

		case CreatorModeMenuItems::CMMI_TELEPORT:
			_AnyTeleportTargets(renderer);
			break;

		default:
			return false;
		}

		return true;
	}

	bool _AnyTeleportTargets(SDL_Renderer* renderer)
	{
		if (blooDot::MakerSettings::GetNumberOfTeleportTargets() == 0)
		{
			GpuChanLoop(
				renderer,
				literalMessageMakerNoTeleportTargets,
				literalDialogTitleGpuChan,
				literalDialogBubbleGpuChanEmpty
			);

			return false;
		}

		return true;
	}
}