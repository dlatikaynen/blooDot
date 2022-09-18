#include "pch.h"
#include "menu-ingame.h"
#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo.h>
#include "xlations.h"
#include "settings.h"
#include "constants.h"
#include "sfx.h"

extern SettingsStruct Settings;
extern Uint32 SDL_USEREVENT_SAVE;
extern Uint32 SDL_USEREVENT_AUTOSAVE;

namespace blooDot::MenuInGame
{
	SDL_Event menuEvent;
	bool menuRunning = false;
	InGameMenuItems menuSelection = IGMI_DISMISS;

	bool MenuLoop(SDL_Renderer* renderer)
	{
		menuRunning = true;

		SDL_Rect outerMenuRect{ 150,45,340,390 };
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
						if (menuSelection == IGMI_DISMISS)
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
						if (menuSelection == IGMI_LEAVE)
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
						if (menuSelection == IGMI_LEAVE)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = IGMI_LEAVE;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_PAGEUP:
					case SDL_SCANCODE_HOME:
						if (menuSelection == IGMI_DISMISS)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = IGMI_DISMISS;
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

		/*	if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = IMG_GetError();
				ReportError("Failed to clear the in-game menu screen", clearError);
			}*/

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

			DrawLabel(renderer, 286, 54, titleTexture, &titleRect);

			const auto drawingTexture = BeginRenderDrawing(renderer, GodsPreferredWidth, GodsPreferredHight);
			if (drawingTexture) [[likely]]
			{
				const auto drawingSink = GetDrawingSink();
				const int stride = 46;
				InGameMenuItems itemToDraw = IGMI_DISMISS;
				for (auto y = 94; y < 400; y += stride)
				{
					DrawButton(drawingSink, 195, y, 250, 42, itemToDraw == menuSelection);
					if (itemToDraw == menuSelection)
					{
						DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
						DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);
					}

					itemToDraw = static_cast<InGameMenuItems>(itemToDraw + 1);
				}

				EndRenderDrawing(renderer, drawingTexture);

				DrawLabel(renderer, 235, 100 + 0 * stride, dismissTexture, &dismissRect);
				DrawLabel(renderer, 235, 100 + 1 * stride, saveTexture, &saveRect);
				DrawLabel(renderer, 235, 100 + 2 * stride, inventoryTexture, &inventoryRect);
				DrawLabel(renderer, 235, 100 + 3 * stride, mapTexture, &mapRect);
				DrawLabel(renderer, 235, 100 + 4 * stride, controlsTexture, &controlsRect);
				DrawLabel(renderer, 235, 100 + 5 * stride, knowledgebaseTexture, &knowledgebaseRect);
				DrawLabel(renderer, 235, 100 + 6 * stride, leaveTexture, &leaveRect);
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
		SaveSettings();

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
		}

		// by default, dismiss
		menuRunning = false;
	}

	void _HandleSave()
	{
		SDL_Event saveEvent = { 0 };
		saveEvent.type = SDL_USEREVENT;
		saveEvent.user.type = SDL_USEREVENT_SAVE;
		if (SDL_PushEvent(&saveEvent) < 0)
		{
			const auto& pushError = SDL_GetError();
			ReportError("Could not post save event", pushError);
		}
	}
}