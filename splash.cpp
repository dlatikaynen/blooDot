#include "pch.h"
#include "splash.h"
#include "dexassy.h"
#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo.h>
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
	MainMenuItems menuSelection = MMI_CUE;

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

			menuSelection = ::Settings.CurrentSavegameIndex == 0 ? MMI_NEWSINGLE : MMI_CUE;
		}

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
			while (SDL_PollEvent(&splashEvent) != 0)
			{
				switch (splashEvent.type)
				{
				case SDL_QUIT:
					mainRunning = false;
					splashRunning = false;
					break;

				case SDL_KEYDOWN:
					switch (splashEvent.key.keysym.scancode)
					{
					case SDL_SCANCODE_UP:
					case SDL_SCANCODE_KP_8:
					case SDL_SCANCODE_W:
						if (menuSelection == MMI_CUE)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = static_cast<MainMenuItems>(menuSelection - 1);
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_DOWN:
					case SDL_SCANCODE_KP_2:
					case SDL_SCANCODE_S:
						if (menuSelection == MMI_EXIT)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = static_cast<MainMenuItems>(menuSelection + 1);
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_PAGEDOWN:
					case SDL_SCANCODE_END:
						if (menuSelection == MMI_EXIT)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = MMI_EXIT;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_PAGEUP:
						if (menuSelection == MMI_CUE)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = MMI_CUE;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_HOME:
					{
						auto const& destSelection = ::Settings.CurrentSavegameIndex == 0 ? MMI_NEWSINGLE : MMI_CUE;
						if (menuSelection == destSelection)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = destSelection;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;
					}

					case SDL_SCANCODE_RETURN:
					case SDL_SCANCODE_RETURN2:
					case SDL_SCANCODE_KP_ENTER:
					case SDL_SCANCODE_SPACE:
						blooDot::Sfx::Play(SoundEffect::SFX_SELCONF);
						keepRunning = _EnterAndHandleMenu(renderer);
						break;

					case SDL_SCANCODE_ESCAPE:
						splashRunning = false;
						keepRunning = false;
					}

					break;

				case SDL_CONTROLLERAXISMOTION:
					if (splashEvent.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
					{
						srcRect.x += splashEvent.caxis.value / 8192;
					}

					if (splashEvent.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
					{
						srcRect.y += splashEvent.caxis.value / 8192;
					}

					break;
				}
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

			DrawLabel(renderer, 286, 54, titleTexture, &titleRect);
			DrawLabel(renderer, 346, 454, authorTexture, &authorRect);

			const auto drawingTexture = BeginRenderDrawing(renderer, GodsPreferredWidth, GodsPreferredHight);
			if (drawingTexture) [[likely]]
			{
				const auto drawingSink = GetDrawingSink();
				const int stride = 46;
				MainMenuItems itemToDraw = MMI_CUE;
				for (auto y = 94; y < 400; y += stride)
				{
					DrawButton(drawingSink, 195, y, 250, 42, itemToDraw == menuSelection);
					if (itemToDraw == menuSelection)
					{
						DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
						DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);
					}

					itemToDraw = static_cast<MainMenuItems>(itemToDraw + 1);
				}

				EndRenderDrawing(renderer, drawingTexture);

				DrawLabel(renderer, 235, 100 + 0 * stride, continueTexture, &continueRect);
				DrawLabel(renderer, 235, 100 + 1 * stride, loadTexture, &loadRect);
				DrawLabel(renderer, 235, 100 + 2 * stride, singleTexture, &singleRect);
				DrawLabel(renderer, 235, 100 + 3 * stride, localMultiTexture, &localMultiRect);
				DrawLabel(renderer, 235, 100 + 4 * stride, netMultiTexture, &netMultiRect);
				DrawLabel(renderer, 235, 100 + 5 * stride, settingsTexture, &settingsRect);
				DrawLabel(renderer, 235, 100 + 6 * stride, quitTexture, &quitRect);
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
			/* we can launch immediately */
			stayInMenu = false;
		}

		return stayInMenu;
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

		return stayInMenu;
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

		return stayInMenu;
	}

	bool _EnterAndHandleMenu(SDL_Renderer* renderer)
	{
		switch (menuSelection)
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