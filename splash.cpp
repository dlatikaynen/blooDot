#include "pch.h"
#include "splash.h"
#include "dexassy.h"
#include "scripture.h"
#include "drawing.h"
#include "menu-common.h"
#include "dialogcontrols.h"
#include <cairo/cairo.h>
#include "xlations.h"
#include <random>
#include <iostream>
#include "settings.h"
#include "constants.h"
#include "sfx.h"
#include "resutil.h"
#include "savegame.h"
#include "menu-continue-empty.h"
#include "menu-load.h"
#include "menu-settings.h"
#include "menu-multimonitor.h"

extern bool mainRunning;
extern SettingsStruct Settings;

namespace blooDot::Splash
{
	SDL_Event splashEvent;
	bool splashRunning = false;
	bool settingsLoaded = false;
	SDL_Texture* splashTexture = NULL;
	int backgroundSpeedX = 0;
	int backgroundSpeedY = 0;
	double backgroundPosX = 0.0;
	double backgroundPosY = 0.0;
	const int backdropHorz = 1280 - GodsPreferredWidth;
	const int backdropVert = 720 - GodsPreferredHight;
	auto backgroundAnimDelay = 237;
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, 3);
	MenuDialogInteraction menuState;

	SDL_Rect continueRect{ 0,0,0,0 };
	SDL_Rect loadRect{ 0,0,0,0 };
	SDL_Rect singleRect{ 0,0,0,0 };
	SDL_Rect localMultiRect{ 0,0,0,0 };
	SDL_Rect netMultiRect{ 0,0,0,0 };
	SDL_Rect settingsRect{ 0,0,0,0 };
	SDL_Rect quitRect{ 0,0,0,0 };

	SDL_Texture* continueTexture = NULL;
	SDL_Texture* loadTexture = NULL;
	SDL_Texture* singleTexture = NULL;
	SDL_Texture* localMultiTexture = NULL;
	SDL_Texture* netMultiTexture = NULL;
	SDL_Texture* settingsTexture = NULL;
	SDL_Texture* quitTexture = NULL;

	void LoadSplash(SDL_Renderer* renderer)
	{
		generator.seed(clock());
		if (!LoadFonts())
		{
			mainRunning = false;
			return;
		}

		if (!settingsLoaded)
		{
			blooDot::Settings::Load();
			blooDot::Settings::ApplyLanguage();
			settingsLoaded = true;
		}

		menuState.itemCount = MMI_EXIT + 1;
		menuState.selectedItemIndex = ::Settings.CurrentSavegameIndex == 0 ? MMI_NEWSINGLE : MMI_CUE;
		_PrepareText(renderer);

		SDL_Rect splashRect;
		splashTexture = blooDot::Res::LoadPicture(renderer, CHUNK_KEY_SPLASH, &splashRect);
		if (!splashTexture)
		{
			mainRunning = false;
			return;
		}

		splashRunning = true;
	}

	bool SplashLoop(SDL_Renderer* renderer)
	{
		bool keepRunning = true;
		LoadSplash(renderer);

		SDL_Rect srcRect{ 0,0,GodsPreferredWidth,GodsPreferredHight };
		SDL_Rect dstRect{ 0,0,GodsPreferredWidth,GodsPreferredHight };
		SDL_Rect outerMenuRect{ 150,45,340,390 };
		SDL_Rect titleRect{ 0,0,0,0 };
		SDL_Rect authorRect{ 0,0,0,0 };

		const auto titleTexture = RenderText(
			renderer,
			&titleRect,
			FONT_KEY_ALIEN,
			26,
			literalAlienMainMenuLabel,
			{ 250,200,200,222 }
		);

		const auto authorTexture = RenderText(
			renderer,
			&authorRect,
			FONT_KEY_ALIEN,
			26,
			literalManufacturer,
			{ 250,200,200,222 }
		);

		const auto primaryController = SDL_GameControllerOpen(0);
		if (primaryController)
		{
			SDL_GameControllerSetPlayerIndex(primaryController, 1);
			SDL_GameControllerRumble(primaryController, 0xffff, 0xffff, 0xff);
			SDL_GameControllerEventState(SDL_ENABLE);
		}

		unsigned short frame = 0L;
		while (splashRunning && mainRunning)
		{
			blooDot::MenuCommon::HandleMenu(&menuState);
			if (menuState.leaveDialog || menuState.leaveMain)
			{
				splashRunning = false;
				mainRunning = false;
				keepRunning = false;
			}
			else if (menuState.enterMenuItem)
			{
				keepRunning = _EnterAndHandleMenu(renderer) && mainRunning;
			}

			_Bounce(&srcRect);

			if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = IMG_GetError();
				ReportError("Failed to clear the title screen", clearError);
			}

			if (SDL_RenderCopy(renderer, splashTexture, &srcRect, &dstRect) < 0)
			{
				const auto blitError = SDL_GetError();
				ReportError("Failed to blit splash background", blitError);
				splashRunning = false;
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
			SDL_RenderFillRect(renderer, &outerMenuRect);
			SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
			if (SDL_RenderDrawRect(renderer, &outerMenuRect) < 0)
			{
				const auto drawRectError = SDL_GetError();
				ReportError("Failed to draw panel border", drawRectError);
				splashRunning = false;
			};

			DrawLabel(renderer, 286, 51, titleTexture, &titleRect);
			DrawLabel(renderer, 346, 451, authorTexture, &authorRect);

			const auto drawingTexture = BeginRenderDrawing(renderer, GodsPreferredWidth, GodsPreferredHight);
			if (drawingTexture) [[likely]]
			{
				const auto drawingSink = GetDrawingSink();
				const int yStart = 94;
				const int stride = 46;
				MainMenuItems itemToDraw = MMI_CUE;
				for (auto y = yStart; y < 400; y += stride)
				{
					auto thisItem = itemToDraw == menuState.selectedItemIndex;
					DrawButton(drawingSink, 195, y, 250, 42, thisItem);
					if (thisItem)
					{
						DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
						DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);
					}

					itemToDraw = static_cast<MainMenuItems>(itemToDraw + 1);
				}

				EndRenderDrawing(renderer, drawingTexture, nullptr);

				DrawLabel(renderer, 235, yStart + 0 * stride, continueTexture, &continueRect);
				DrawLabel(renderer, 235, yStart + 1 * stride, loadTexture, &loadRect);
				DrawLabel(renderer, 235, yStart + 2 * stride, singleTexture, &singleRect);
				DrawLabel(renderer, 235, yStart + 3 * stride, localMultiTexture, &localMultiRect);
				DrawLabel(renderer, 235, yStart + 4 * stride, netMultiTexture, &netMultiRect);
				DrawLabel(renderer, 235, yStart + 5 * stride, settingsTexture, &settingsRect);
				DrawLabel(renderer, 235, yStart + 6 * stride, quitTexture, &quitRect);
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		if (primaryController)
		{
			SDL_GameControllerClose(primaryController);
		}

		splashTexture && [] { SDL_DestroyTexture(splashTexture); return false;  }();
		titleTexture && [titleTexture] { SDL_DestroyTexture(titleTexture); return false;  }();
		authorTexture && [authorTexture] { SDL_DestroyTexture(authorTexture); return false;  }();

		_PrepareText(renderer, true);

		return keepRunning;
	}

	void _Bounce(SDL_Rect* srcRect)
	{
		if (backgroundSpeedX == 0)
		{
			_AssignNewSpeed(&backgroundSpeedX);
		}

		if (backgroundSpeedY == 0)
		{
			_AssignNewSpeed(&backgroundSpeedY);
		}

		if (backgroundAnimDelay > 0)
		{
			--backgroundAnimDelay;
			return;
		}

		backgroundPosX += (double)backgroundSpeedX * 0.5;
		backgroundPosY += (double)backgroundSpeedY * 0.5;

		if (backgroundPosX < 0)
		{
			backgroundPosX = 0;
			_AssignNewSpeed(&backgroundSpeedX);
			backgroundSpeedX = abs(backgroundSpeedX);
			_DelayBackgroundAnim();
		}
		else if (backgroundPosX > backdropHorz)
		{
			backgroundPosX = backdropHorz;
			_AssignNewSpeed(&backgroundSpeedX);
			backgroundSpeedX = -abs(backgroundSpeedX);
			_DelayBackgroundAnim();
		}

		if (backgroundPosY < 0)
		{
			backgroundPosY = 0;
			_AssignNewSpeed(&backgroundSpeedY);
			backgroundSpeedY = abs(backgroundSpeedY);
			_DelayBackgroundAnim();
		}
		else if (backgroundPosY > backdropVert)
		{
			backgroundPosY = backdropVert;
			_AssignNewSpeed(&backgroundSpeedY);
			backgroundSpeedY = -abs(backgroundSpeedY);
			_DelayBackgroundAnim();
		}

		(*srcRect).x = (int)round(backgroundPosX);
		(*srcRect).y = (int)round(backgroundPosY);
	}

	void _AssignNewSpeed(__out int* speed)
	{
		const auto value = distribution(generator);
		if (value < 2)
		{
			/* -2, -1 */
			(*speed) = value - 2;
		}
		else
		{
			/* 1, 2 */
			(*speed) = value - 1;
		}
	}

	void _DelayBackgroundAnim()
	{
		const auto random = distribution(generator);
		backgroundAnimDelay = 101 * (random + 1);
	}

	void _PrepareText(SDL_Renderer* renderer, bool destroy)
	{
		continueTexture && [] { SDL_DestroyTexture(continueTexture); return false; }();
		loadTexture && [] { SDL_DestroyTexture(loadTexture); return false; }();
		singleTexture && [] { SDL_DestroyTexture(singleTexture); return false; }();
		localMultiTexture && [] { SDL_DestroyTexture(localMultiTexture); return false; }();
		netMultiTexture && [] { SDL_DestroyTexture(netMultiTexture); return false; }();
		settingsTexture && [] { SDL_DestroyTexture(settingsTexture); return false; }();
		quitTexture && [] { SDL_DestroyTexture(quitTexture); return false; }();

		if (!destroy)
		{
			continueTexture = RenderText(renderer, &continueRect, FONT_KEY_DIALOG, 23, literalMenuContinue, { 250, 230, 230, 245 });
			loadTexture = RenderText(renderer, &loadRect, FONT_KEY_DIALOG, 23, literalMenuLoad, { 250, 230, 230, 245 });
			singleTexture = RenderText(renderer, &singleRect, FONT_KEY_DIALOG, 23, literalMenuSingle, { 250, 230, 230, 245 });
			localMultiTexture = RenderText(renderer, &localMultiRect, FONT_KEY_DIALOG, 23, literalMenuLocalMulti, { 250, 230, 230, 245 });
			netMultiTexture = RenderText(renderer, &netMultiRect, FONT_KEY_DIALOG, 23, literalMenuNetMulti, { 250, 230, 230, 245 });
			settingsTexture = RenderText(renderer, &settingsRect, FONT_KEY_DIALOG, 23, literalMenuSettings, { 250, 230, 230, 245 });
			quitTexture = RenderText(renderer, &quitRect, FONT_KEY_DIALOG, 23, literalMenuExit, { 250, 230, 230, 245 });
		}
	}

	bool _HandleContinue(SDL_Renderer* renderer)
	{
		bool stayInMenu = true;
		if (::Settings.CurrentSavegameIndex == 0)
		{
			if (::Settings.OccupiedSavegameSlots == 0)
			{
				/* there is no save game to choose from */
				blooDot::MenuContinueEmpty::MenuLoop(renderer, literalContinueEmptyHint);
			}
			else
			{
				/* offer to choose from savegames to continue
				 * (essentially, this is the same further workflow as ::Load) */

			}
		}
		else
		{
			/* we can launch now */
			stayInMenu = false;
		}

		return _HandleLaunch(renderer, stayInMenu);
	}

	bool _HandleLoad(SDL_Renderer* renderer)
	{
		bool stayInMenu = true;
		if (::Settings.OccupiedSavegameSlots == 0)
		{
			/* there is no save game to load from */
			blooDot::MenuContinueEmpty::MenuLoop(renderer, literalLoadEmptyHint);
		}
		else
		{
			/* we show the list of savegames to load from */
			blooDot::MenuLoad::MenuLoop(renderer);
			stayInMenu = true;
		}

		return _HandleLaunch(renderer, stayInMenu);
	}

	bool _HandleNew(SDL_Renderer* renderer)
	{
		bool stayInMenu = false;
		const auto& newSavegameIndex = blooDot::Savegame::Create();
		if (newSavegameIndex == 0)
		{
			/* no free slot. ask to overwrite something else */

			stayInMenu = renderer != NULL;
		}

		return _HandleLaunch(renderer, stayInMenu);
	}

	/// <summary>
	/// The point of this function is to have a hook for the 
	/// multi-display choice submenu immediately before we
	/// launch into the arena
	/// </summary>
	bool _HandleLaunch(SDL_Renderer* renderer, bool stayInMenu)
	{
		if (!stayInMenu)
		{
			if (SDL_GetNumVideoDisplays() > 1)
			{
				if (!blooDot::MultiMonitorMenuScreen::MenuLoop(renderer))
				{
					/* bit confusing.but it means, that we left the dialog
					 * with the cancel button, so the intention is to stay
					 * in the splash menu, as opposed to leave it for the arena */
					stayInMenu = true;
				}
			}
		}

		return stayInMenu;
	}

	bool _EnterAndHandleMenu(SDL_Renderer* renderer)
	{
		switch (menuState.selectedItemIndex)
		{
		case MMI_CUE:
			if (!_HandleContinue(renderer))
			{
				splashRunning = false;
			}

			break;

		case MMI_LOAD:
			if (!_HandleLoad(renderer))
			{
				splashRunning = false;
			}

			break;

		case MMI_NEWSINGLE:
			if (!_HandleNew(renderer))
			{
				splashRunning = false;
			}

			break;

		case MMI_SETTINGS:
			_EnterAndHandleSettings(renderer);
			break;

		case MMI_EXIT:
			splashRunning = false;
			return false;

		default:
			splashRunning = false;
			break;
		}

		return true;
	}

	void _EnterAndHandleSettings(SDL_Renderer* renderer)
	{
		const auto& lcidBefore = std::string(literalLCID);
		blooDot::MenuSettings::MenuLoop(renderer);
		
		// because language might have changed
		const auto& lcidAfter = std::string(literalLCID);
		if (lcidAfter != lcidBefore)
		{
			_PrepareText(renderer);
		}
	}
}