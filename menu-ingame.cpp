#include "pch.h"
#include "menu-ingame.h"
#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo/cairo.h>
#include "xlations.h"
#include "settings.h"
#include "constants.h"
#include "sfx.h"

extern SettingsStruct Settings;
extern Uint32 SDL_USEREVENT_SAVE;
extern Uint32 SDL_USEREVENT_AUTOSAVE;
extern Uint32 SDL_USEREVENT_LEAVE;

namespace blooDot::MenuInGame
{
	SDL_Event menuEvent;
	bool menuRunning = false;
	InGameMenuItems menuSelection = IGMI_DISMISS;

	bool MenuLoop(SDL_Renderer* renderer)
	{
		const auto& physicalW = blooDot::Settings::GetPhysicalArenaWidth();
		const auto& physicalH = blooDot::Settings::GetPhysicalArenaHeight();

		menuRunning = true;

		SDL_Rect outerMenuRect{ physicalW / 2 - 340 / 2, physicalH / 2 - 390 / 2,340,390 };
		SDL_Rect titleRect{ 0,0,0,0 };
		SDL_Rect dismissRect{ 0,0,0,0 };
		SDL_Rect saveRect{ 0,0,0,0 };
		SDL_Rect inventoryRect{ 0,0,0,0 };
		SDL_Rect mapRect{ 0,0,0,0 };
		SDL_Rect controlsRect{ 0,0,0,0 };
		SDL_Rect knowledgebaseRect{ 0,0,0,0 };
		SDL_Rect leaveRect{ 0,0,0,0 };

		const auto titleTexture = RenderText(
			renderer,
			&titleRect,
			FONT_KEY_ALIEN,
			26,
			literalAlienInGameMenuLabel,
			AlienTextColor
		);

		/* menu item text */
		const auto dismissTexture = RenderText(
			renderer,
			&dismissRect,
			FONT_KEY_DIALOG,
			23,
			literalMenuDismiss,
			ButtonTextColor
		);

		const auto saveTexture = RenderText(
			renderer,
			&saveRect,
			FONT_KEY_DIALOG,
			23,
			literalMenuSave,
			ButtonTextColor
		);

		const auto inventoryTexture = RenderText(
			renderer,
			&inventoryRect,
			FONT_KEY_DIALOG,
			23,
			literalMenuInventory,
			ButtonTextColor
		);

		const auto mapTexture = RenderText(
			renderer,
			&mapRect,
			FONT_KEY_DIALOG,
			23,
			literalMenuMap,
			ButtonTextColor
		);

		const auto controlsTexture = RenderText(
			renderer,
			&controlsRect,
			FONT_KEY_DIALOG,
			23,
			literalMenuControls,
			ButtonTextColor
		);

		const auto knowledgebaseTexture = RenderText(
			renderer,
			&knowledgebaseRect,
			FONT_KEY_DIALOG,
			23,
			literalMenuKnowledgebase,
			ButtonTextColor
		);

		const auto leaveTexture = RenderText(
			renderer,
			&leaveRect,
			FONT_KEY_DIALOG,
			23,
			literalMenuLeave,
			ButtonTextColor
		);

		unsigned short frame = 0;
		constexpr auto topItem = IGMI_DISMISS;
		constexpr auto bottomItem = IGMI_LEAVE;
		while (menuRunning)
		{
			while (SDL_PollEvent(&menuEvent) != 0)
			{
				switch (menuEvent.type)
				{
				case SDL_QUIT:
					menuRunning = false;
					break;

				case SDL_KEYDOWN:
					switch (menuEvent.key.keysym.scancode)
					{
					case SDL_SCANCODE_UP:
					case SDL_SCANCODE_KP_8:
					case SDL_SCANCODE_W:
						if (menuSelection == topItem)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = static_cast<InGameMenuItems>(menuSelection - 1);
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_DOWN:
					case SDL_SCANCODE_KP_2:
					case SDL_SCANCODE_S:
						if (menuSelection == bottomItem)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = static_cast<InGameMenuItems>(menuSelection + 1);
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_PAGEDOWN:
					case SDL_SCANCODE_END:
						if (menuSelection == bottomItem)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = bottomItem;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_PAGEUP:
					case SDL_SCANCODE_HOME:
						if (menuSelection == topItem)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = topItem;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_RETURN:
					case SDL_SCANCODE_RETURN2:
					case SDL_SCANCODE_KP_ENTER:
					case SDL_SCANCODE_SPACE:
						_HandleMenu();
						break;

					case SDL_SCANCODE_ESCAPE:
						menuRunning = false;
						break;
					}

					break;
				}
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
			SDL_RenderFillRect(renderer, &outerMenuRect);
			SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
			if (SDL_RenderDrawRect(renderer, &outerMenuRect) < 0)
			{
				const auto drawRectError = SDL_GetError();
				ReportError("Failed to draw in-game menu panel border", drawRectError);
				menuRunning = false;
			};

			DrawLabel(renderer, outerMenuRect.x + 133, outerMenuRect.y + 9, titleTexture, &titleRect);

			const auto drawingTexture = BeginRenderDrawing(renderer, physicalW, physicalH);
			if (drawingTexture)
			{
				const auto drawingSink = GetDrawingSink();
				const auto firstY = outerMenuRect.y + 49;
				const auto bottom = outerMenuRect.y + 355;
				const auto left = outerMenuRect.x + 45;
				const auto labelLeft = outerMenuRect.x + 85;
				constexpr const int stride = 46;
				InGameMenuItems itemToDraw = IGMI_DISMISS;
				for (auto y = firstY; y < bottom; y += stride)
				{
					DrawButton(drawingSink, left, y, 250, 42, itemToDraw == menuSelection ? CH_INGAME : CH_NONE);
					if (itemToDraw == menuSelection)
					{
						DrawChevron(drawingSink, left - 7, y + 21, false, frame);
						DrawChevron(drawingSink, left + 250 + 7, y + 21, true, frame);
					}

					itemToDraw = static_cast<InGameMenuItems>(itemToDraw + 1);
				}

				EndRenderDrawing(renderer, drawingTexture, nullptr);

				DrawLabel(renderer, labelLeft, firstY + 0 * stride, dismissTexture, &dismissRect);
				DrawLabel(renderer, labelLeft, firstY + 1 * stride, saveTexture, &saveRect);
				DrawLabel(renderer, labelLeft, firstY + 2 * stride, inventoryTexture, &inventoryRect);
				DrawLabel(renderer, labelLeft, firstY + 3 * stride, mapTexture, &mapRect);
				DrawLabel(renderer, labelLeft, firstY + 4 * stride, controlsTexture, &controlsRect);
				DrawLabel(renderer, labelLeft, firstY + 5 * stride, knowledgebaseTexture, &knowledgebaseRect);
				DrawLabel(renderer, labelLeft, firstY + 6 * stride, leaveTexture, &leaveRect);
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		SDL_DestroyTexture(titleTexture);
		SDL_DestroyTexture(dismissTexture);
		SDL_DestroyTexture(saveTexture);
		SDL_DestroyTexture(inventoryTexture);
		SDL_DestroyTexture(mapTexture);
		SDL_DestroyTexture(controlsTexture);
		SDL_DestroyTexture(knowledgebaseTexture);
		SDL_DestroyTexture(leaveTexture);
		blooDot::Settings::Save();

		return menuRunning;
	}

	void _HandleMenu()
	{
		blooDot::Sfx::Play(SoundEffect::SFX_SELCONF);
		switch (menuSelection)
		{
		case InGameMenuItems::IGMI_SAVE:
			_HandleSave();
			break;

		case InGameMenuItems::IGMI_LEAVE:
			_HandleLeave();
			break;
		}

		// by default, dismiss
		menuRunning = false;
	}

	void _HandleSave()
	{
		SDL_Event saveEvent = { 0 };
		saveEvent.type = SDL_USEREVENT_SAVE;
		if (SDL_PushEvent(&saveEvent) < 0)
		{
			const auto& pushError = SDL_GetError();
			ReportError("Could not post save event", pushError);
		}
	}

	void _HandleLeave()
	{
		SDL_Event leaveEvent = { 0 };
		leaveEvent.type = SDL_USEREVENT_LEAVE;
		if (SDL_PushEvent(&leaveEvent) < 0)
		{
			const auto& pushError = SDL_GetError();
			ReportError("Could not post leave event", pushError);
		}
	}
}