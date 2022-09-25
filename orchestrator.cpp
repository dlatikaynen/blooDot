#include "pch.h"
#include "orchestrator.h"
#include "menu-ingame.h"
#include "settings.h"
#include "savegame.h"
#include "constants.h"

Uint32 SDL_USEREVENT_SAVE = 0;
Uint32 SDL_USEREVENT_AUTOSAVE = 0;

extern SettingsStruct Settings;
extern SDL_Renderer* GameViewRenderer;

bool mainRunning = true;
#ifndef NDEBUG
bool toggleDebugView = false;
#endif

namespace blooDot::Orchestrator
{
	SDL_Event mainEvent;

	void MainLoop(SDL_Renderer* renderer)
	{
		if (!InitializeNewWorld())
		{
			mainRunning = false;
		}

		GameViewRenderer = renderer;
		if (mainRunning && !GameviewEnterWorld())
		{
			mainRunning = false;
		}

		SDL_USEREVENT_SAVE = SDL_RegisterEvents(1);
		SDL_USEREVENT_AUTOSAVE = SDL_RegisterEvents(1);
		
		Uint64 frameEnded;
		Uint64 frameStart;
		const auto& preMultiplied = (float)SDL_GetPerformanceFrequency();
#ifndef NDEBUG
		long long frameNumber = 0;
#endif

		while (mainRunning)
		{
			frameStart = SDL_GetPerformanceCounter();
			while (SDL_PollEvent(&mainEvent) != 0)
			{
				switch (mainEvent.type)
				{
				case SDL_USEREVENT:
				{
					const auto& userEvent = mainEvent.user;
					if (userEvent.type == SDL_USEREVENT_SAVE)
					{
						_HandleSave();
					}
					else if (userEvent.type == SDL_USEREVENT_AUTOSAVE)
					{
						_HandleSave(true);
					}

					break;
				}

				case SDL_KEYDOWN:
					switch (mainEvent.key.keysym.scancode)
					{
#ifndef NDEBUG
					case SDL_SCANCODE_LEFTBRACKET:
						/* this...is...the...bracket */
						toggleDebugView = !toggleDebugView;
						break;

#endif
					case SDL_SCANCODE_LEFT:
					case SDL_SCANCODE_KP_4:
						Scroll(-5, 0);
						break;

					case SDL_SCANCODE_RIGHT:
					case SDL_SCANCODE_KP_6:
						Scroll(5, 0);
						break;

					case SDL_SCANCODE_UP:
					case SDL_SCANCODE_KP_8:
						Scroll(0, -5);
						break;

					case SDL_SCANCODE_DOWN:
					case SDL_SCANCODE_KP_2:
						Scroll(0, 5);
						break;

					case SDL_SCANCODE_KP_7:
						Scroll(-5, -5);
						break;

					case SDL_SCANCODE_KP_9:
						Scroll(5, -5);
						break;

					case SDL_SCANCODE_KP_1:
						Scroll(-5, 5);
						break;

					case SDL_SCANCODE_KP_3:
						Scroll(5, 5);
						break;

					case SDL_SCANCODE_ESCAPE:
						blooDot::MenuInGame::MenuLoop(renderer);
						goto NEXTFRAME;
					}

					break;

				case SDL_QUIT:
					mainRunning = false;
					goto THAT_ESCALATED_QUICKLY;
				}
			}
NEXTFRAME:

			SDL_RenderClear(renderer);
			GameViewRenderFrame();
			SDL_RenderPresent(renderer);
			frameEnded = SDL_GetPerformanceCounter();
			const auto& frameTime = (frameEnded - frameStart) / preMultiplied * 1000.f;
			const auto& frameSlack = static_cast<int>(MillisecondsPerFrame - frameTime);
			if (frameSlack > 0)
			{
				SDL_Delay(frameSlack);
			}

#ifndef NDEBUG
			if (++frameNumber % 60 == 0)
			{
				std::cout << "FRAME TIME " << frameTime << "\n";
			}
#endif
		}

THAT_ESCALATED_QUICKLY:
		TeardownDingSheets();
		GameviewTeardown();
		ClearWorldData();
	}

	void _HandleSave(bool isAutosave)
	{
		const auto& savegameIndex = ::Settings.CurrentSavegameIndex;
		if (savegameIndex <= 0)
		{
			std::cerr << "Cannot save as long as no savegame index is established";
			return;
		}

		/* 1. take a screenshot */
		const int shotWidth = 200, shotHeight = 120;
		int viewportWidth = 0, viewportHeight = 0;
		SDL_RWops* screenshotStream = NULL;
		size_t screenshotLength = 0;
		void* memoryBuffer = NULL;
		
		SDL_GetRendererOutputSize(GameViewRenderer, &viewportWidth, &viewportHeight);
		SDL_Surface* screenShot = SDL_CreateRGBSurface(
			0,
			shotWidth,
			shotHeight,
			32,
			0x00ff0000,
			0x0000ff00,
			0x000000ff,
			0xff000000
		);

		if (screenShot)
		{
			const auto& srcRect = SDL_Rect({
				viewportWidth / 2 - shotWidth / 2,
				viewportHeight / 2 - shotHeight / 2,
				shotWidth,
				shotHeight
			});

			if (SDL_RenderReadPixels(
				GameViewRenderer,
				&srcRect,
				SDL_PIXELFORMAT_ARGB8888,
				screenShot->pixels,
				screenShot->pitch
			) < 0)
			{
				const auto& shotError = SDL_GetError();
				ReportError("Could not take screenshot", shotError);
			}
			else
			{
				const auto& screenshotSize = shotWidth * shotHeight * 32;
				memoryBuffer = SDL_malloc(screenshotSize);
				if (memoryBuffer)
				{
					screenshotStream = SDL_RWFromMem(memoryBuffer, screenshotSize);
					if (screenshotStream)
					{
						if (IMG_SavePNG_RW(screenShot, screenshotStream, 0) < 0)
						{
							const auto& pngError = IMG_GetError();
							ReportError("Failed to convert screenshot", pngError);
						}
						else
						{
							screenshotLength = screenshotStream->seek(screenshotStream, 0, RW_SEEK_CUR);
							screenshotStream->seek(screenshotStream, 0, RW_SEEK_SET);
						}
					}
					else
					{
						const auto& convertError = SDL_GetError();
						ReportError("Error writing converted screenshot", convertError);
					}
				}
				else
				{
					const auto& allocError = SDL_GetError();
					ReportError("Failed to allocate memory for screenshot conversion", allocError);
				}
			}
		}
		else
		{
			const auto& surfaceError = SDL_GetError();
			ReportError("Could not create surface for screenshot", surfaceError);
		}

		/* 2. in a separate thread,
		 * which, while it will be running,
		 * no new save can be initiated,
		 * write the data to the savegame file */
		blooDot::Savegame::Append(::Settings.CurrentSavegameIndex, isAutosave, screenshotLength, memoryBuffer);

		if (memoryBuffer)
		{
			SDL_free(memoryBuffer);
		}

		if (screenshotStream)
		{
			screenshotStream->close(screenshotStream);
		}

		if (screenShot)
		{
			SDL_FreeSurface(screenShot);
		}
	}
}