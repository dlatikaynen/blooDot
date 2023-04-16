#include "resutil.h"
#include "pch.h"
#include "menu-continue-empty.h"
#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo.h>
#include "xlations.h"
#include "constants.h"
#include "sfx.h"
#include "resutil.h"

extern bool mainRunning;

namespace blooDot::MenuContinueEmpty
{
	SDL_Event continueEmptyEvent;
	bool continueEmptyRunning = false;

	bool MenuLoop(SDL_Renderer* renderer, const char* literalHint)
	{
		continueEmptyRunning = true;

		SDL_Rect outerMenuRect{ 150,45,340,390 };
		SDL_Rect titleRect{ 0,0,0,0 };
		SDL_Rect backRect{ 0,0,0,0 };
		SDL_Rect hintRect{ 0,0,0,0 };
		SDL_Rect screensizeRect{ 0,0,0,0 };
		SDL_Rect controlsRect{ 0,0,0,0 };
		SDL_Rect languageRect{ 0,0,0,0 };
		SDL_Rect tumbleRect{ 0,0,0,0 };

		const auto titleTexture = RenderText(
			renderer,
			&titleRect,
			FONT_KEY_ALIEN,
			26,
			literalAlienContinueEmptyLabel,
			AlienTextColor
		);

		const auto tumbleTexture = blooDot::Res::LoadPicture(renderer, CHUNK_KEY_UI_MENU_EMPTY, &tumbleRect);
		if (!tumbleTexture)
		{
			continueEmptyRunning = false;
		}

		/* menu item text */
		const auto backTexture = RenderText(
			renderer,
			&backRect,
			FONT_KEY_DIALOG_FAT,
			23,
			literalMenuBack,
			ButtonTextColor
		);

		/* explanation why and what to do */
		const auto hintTexture = RenderText(
			renderer,
			&hintRect,
			FONT_KEY_DIALOG,
			13,
			literalHint,
			DialogTextColor
		);

		const auto hintShadow = RenderText(
			renderer,
			&hintRect,
			FONT_KEY_DIALOG,
			13,
			literalHint,
			DialogTextShadowColor
		);

		unsigned short frame = 0;
		while (continueEmptyRunning && mainRunning)
		{
			while (SDL_PollEvent(&continueEmptyEvent) != 0)
			{
				switch (continueEmptyEvent.type)
				{
				case SDL_QUIT:
					mainRunning = false;
					continueEmptyRunning = false;
					break;

				case SDL_KEYDOWN:
					switch (continueEmptyEvent.key.keysym.scancode)
					{
					case SDL_SCANCODE_UP:
					case SDL_SCANCODE_KP_8:
					case SDL_SCANCODE_W:
					case SDL_SCANCODE_DOWN:
					case SDL_SCANCODE_KP_2:
					case SDL_SCANCODE_S:
						blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						break;

					case SDL_SCANCODE_RETURN:
					case SDL_SCANCODE_RETURN2:
					case SDL_SCANCODE_KP_ENTER:
					case SDL_SCANCODE_SPACE:
						blooDot::Sfx::Play(SoundEffect::SFX_SELCONF);
						continueEmptyRunning = false;
						break;

					case SDL_SCANCODE_ESCAPE:
						continueEmptyRunning = false;
						break;
					}

					break;
				}
			}

			if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = IMG_GetError();
				ReportError("Failed to clear the continue on empty screen", clearError);
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
			SDL_RenderFillRect(renderer, &outerMenuRect);
			SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
			if (SDL_RenderDrawRect(renderer, &outerMenuRect) < 0)
			{
				const auto drawRectError = SDL_GetError();
				ReportError("Failed to draw continue on empty menu panel border", drawRectError);
				continueEmptyRunning = false;
			};

			DrawLabel(renderer, 286, 51, titleTexture, &titleRect);

			const auto drawingTexture = BeginRenderDrawing(renderer, GodsPreferredWidth, GodsPreferredHight);
			if (drawingTexture) [[likely]]
			{
				const auto drawingSink = GetDrawingSink();
				const auto y = 94;
				DrawButton(drawingSink, 195, y, 250, 42, true);
				DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
				DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);

				EndRenderDrawing(renderer, drawingTexture);

				DrawLabel(renderer, 235, 97, backTexture, &backRect);
				DrawLabel(renderer, 181, 389, hintShadow, &hintRect);
				DrawLabel(renderer, 180, 388, hintTexture, &hintRect);

				SDL_RenderCopy(renderer, tumbleTexture, &tumbleRect, &tumbleRect);
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;

			if (frame % 128 == 0)
			{
				blooDot::Sfx::Play(SoundEffect::SFX_CRICKET_FRANZ);
			}
			else if (frame % 156 == 0)
			{
				blooDot::Sfx::Play(SoundEffect::SFX_CRICKET_FRITZ);
			}
		}

		titleTexture&& [titleTexture] { SDL_DestroyTexture(titleTexture); return false; }();
		tumbleTexture&& [tumbleTexture] { SDL_DestroyTexture(tumbleTexture); return false; }();
		backTexture&& [backTexture] { SDL_DestroyTexture(backTexture); return false; }();
		hintTexture&& [hintTexture] { SDL_DestroyTexture(hintTexture); return false; }();
		hintShadow&& [hintShadow] { SDL_DestroyTexture(hintShadow); return false; }();

		return continueEmptyRunning;
	}
}