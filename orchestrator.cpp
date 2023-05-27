#include "pch.h"
#include "orchestrator.h"
#include "menu-ingame.h"
#include "settings.h"
#include "savegame.h"
#include "constants.h"

#ifndef NDEBUG
#include "physicsdebugdraw.h"
#endif

Uint32 SDL_USEREVENT_SAVE = 0;
Uint32 SDL_USEREVENT_AUTOSAVE = 0;
Uint32 SDL_USEREVENT_LEAVE = 0; // [sic], they're only allocated later

extern SettingsStruct Settings;
extern SDL_Renderer* GameViewRenderer;

/* for screens where we render only a part (square) */
extern int viewportOffsetX;
extern int viewportOffsetY;


bool mainRunning = true;
#ifndef NDEBUG
bool toggleDebugView = false;
#endif

namespace blooDot::Orchestrator
{
	SDL_Event mainEvent;
	b2Vec2 gravity(0.f, 0.f);
	b2World world(gravity);
	b2BodyDef bodyDef, wallDef;
	b2CircleShape pShape;
	b2PolygonShape wallShape;
	b2FixtureDef fixtureDef, wallFixtureDef;
	float timeStep = MillisecondsPerFrame / 1000.f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

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
		else
		{
			/* player physics */
			EnsurePlayers();

			const auto& p2 = Player::GetState(iP2);
			const auto& p4 = Player::GetState(iP4);

			bodyDef.type = b2_dynamicBody;
			bodyDef.linearDamping = 6.68f;
			bodyDef.position.Set(
				static_cast<float>(p2->Offset.x + GRIDUNIT / 2) / static_cast<float>(GRIDUNIT),
				static_cast<float>(p2->Offset.y + GRIDUNIT / 2) / static_cast<float>(GRIDUNIT)
			);

			const auto& p2Body = world.CreateBody(&bodyDef);

			bodyDef.position.Set(
				(p4->Offset.x + GRIDUNIT / 2) / static_cast<float>(GRIDUNIT),
				(p4->Offset.y + GRIDUNIT / 2) / static_cast<float>(GRIDUNIT)
			);

			const auto& p4Body = world.CreateBody(&bodyDef);

			pShape.m_radius = .33f;
			fixtureDef.shape = &pShape;
			fixtureDef.density = 1.6f * static_cast<float>(M_PI);
			fixtureDef.friction = 0.3f;
			fixtureDef.restitution = .27f;
			p2Body->CreateFixture(&fixtureDef);
			p4Body->CreateFixture(&fixtureDef);
			/* wall physics */
			AttachWorldPhysics(
				&world,
				12 * GRIDUNIT - 29 + GRIDUNIT / 2 + viewportOffsetX,
				12 * GRIDUNIT - 29 + GRIDUNIT / 2 + viewportOffsetY
			);

			/* input */
			SDL_USEREVENT_SAVE = SDL_RegisterEvents(1);
			SDL_USEREVENT_AUTOSAVE = SDL_RegisterEvents(1);
			SDL_USEREVENT_LEAVE = SDL_RegisterEvents(1);

			Uint64 frameEnded;
			Uint64 frameStart;
			const auto& preMultiplied = (float)SDL_GetPerformanceFrequency();
#ifndef NDEBUG
			long long frameNumber = 0;

			PhysicsDebugDraw physicsDebugDraw(GameViewRenderer);
			physicsDebugDraw.SetFlags(0xffff);
			world.SetDebugDraw(&physicsDebugDraw);
#endif

			int numKeys;
			SDL_GetKeyboardState(&numKeys);
			while (mainRunning)
			{
				frameStart = SDL_GetPerformanceCounter();
				while (SDL_PollEvent(&mainEvent) != 0)
				{
					switch (mainEvent.type)
					{
#ifndef NDEBUG
					case SDL_KEYUP:
						if (mainEvent.key.keysym.scancode == SDL_SCANCODE_F6)
						{
							toggleDebugView = !toggleDebugView;
						}

						break;

#endif
					case SDL_QUIT:
					LEAVE:
						mainRunning = false;
						goto THAT_ESCALATED_QUICKLY;
					}

					if (mainEvent.type == SDL_USEREVENT_SAVE)
					{
						_HandleSave();
						continue;
					}
					else if (mainEvent.type == SDL_USEREVENT_LEAVE)
					{
						goto LEAVE;
					}
				}

				if (numKeys > 0)
				{
					const auto& keys = SDL_GetKeyboardState(NULL);

					if (keys[SDL_SCANCODE_A])
					{
						NudgePlayer(iP1, -5, 0);
					}
					else if (keys[SDL_SCANCODE_D])
					{
						NudgePlayer(iP1, 5, 0);
					}

					if (keys[SDL_SCANCODE_S])
					{
						NudgePlayer(iP1, 0, 5);
					}
					else if (keys[SDL_SCANCODE_W])
					{
						NudgePlayer(iP1, 0, -5);
					}

					if (keys[SDL_SCANCODE_F])
					{
						p2Body->ApplyLinearImpulseToCenter({ -1.7f, 0 }, true);
					}

					if (keys[SDL_SCANCODE_H])
					{
						p2Body->ApplyLinearImpulseToCenter({ 1.7f, 0 }, true);
					}

					if (keys[SDL_SCANCODE_G])
					{
						p2Body->ApplyLinearImpulseToCenter({ 0, 1.7f }, true);
					}

					if (keys[SDL_SCANCODE_T])
					{
						p2Body->ApplyLinearImpulseToCenter({ 0, -1.7f }, true);
					}

					if (keys[SDL_SCANCODE_J])
					{
						NudgePlayer(iP3, -5, 0);
					}

					if (keys[SDL_SCANCODE_L])
					{
						NudgePlayer(iP3, 5, 0);
					}

					if (keys[SDL_SCANCODE_K])
					{
						NudgePlayer(iP3, 0, 5);
					}

					if (keys[SDL_SCANCODE_I])
					{
						NudgePlayer(iP3, 0, -5);
					}

					if (keys[SDL_SCANCODE_SEMICOLON])
					{
						p4Body->ApplyLinearImpulseToCenter({ -1.7f, 0 }, true);
					}

					if (keys[SDL_SCANCODE_BACKSLASH])
					{
						p4Body->ApplyLinearImpulseToCenter({ 1.7f, 0 }, true);
					}

					if (keys[SDL_SCANCODE_APOSTROPHE])
					{
						p4Body->ApplyLinearImpulseToCenter({ 0, 1.7f }, true);
					}

					if (keys[SDL_SCANCODE_LEFTBRACKET])
					{
						p4Body->ApplyLinearImpulseToCenter({ 0, -1.7f }, true);
					}

					if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_KP_4])
					{
						Scroll(-5, 0);
						world.ShiftOrigin({ -5 / static_cast<float>(GRIDUNIT),0 });
					}

					if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_KP_6])
					{
						Scroll(5, 0);
						world.ShiftOrigin({ 5 / static_cast<float>(GRIDUNIT),0 });
					}

					if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_KP_8])
					{
						Scroll(0, -5);
						world.ShiftOrigin({ 0, -5 / static_cast<float>(GRIDUNIT) });
					}

					if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_KP_2])
					{
						Scroll(0, 5);
						world.ShiftOrigin({ 0, 5 / static_cast<float>(GRIDUNIT) });
					}

					if (keys[SDL_SCANCODE_KP_7])
					{
						Scroll(-5, -5);
					}

					if (keys[SDL_SCANCODE_KP_9])
					{
						Scroll(5, -5);
					}

					if (keys[SDL_SCANCODE_KP_1])
					{
						Scroll(-5, 5);
					}

					if (keys[SDL_SCANCODE_KP_3])
					{
						Scroll(5, 5);
					}

					if (keys[SDL_SCANCODE_ESCAPE])
					{
						blooDot::MenuInGame::MenuLoop(renderer);
						goto NEXTFRAME;
					}
				}

				// rare use for a scope block, guess why
				{
					world.Step(timeStep, velocityIterations, positionIterations);
					const auto& p2Position = p2Body->GetPosition();
					SetPlayerPosition(
						iP2,
						(int)(p2Position.x* static_cast<float>(GRIDUNIT)) - GRIDUNIT / 2,
						(int)(p2Position.y* static_cast<float>(GRIDUNIT)) - GRIDUNIT / 2
					);

					if (p2Body->GetLinearVelocity().Length() < 0.3f)
					{
						p2Body->SetLinearVelocity({ 0,0 });
					}

					const auto& p4Position = p4Body->GetPosition();
					SetPlayerPosition(
						iP4,
						(int)(p4Position.x* static_cast<float>(GRIDUNIT)) - GRIDUNIT / 2,
						(int)(p4Position.y* static_cast<float>(GRIDUNIT)) - GRIDUNIT / 2
					);

					if (p4Body->GetLinearVelocity().Length() < 0.3f)
					{
						p4Body->SetLinearVelocity({ 0,0 });
					}
				}

			NEXTFRAME:
				SDL_RenderClear(renderer);
				GameViewRenderFrame();
#ifndef NDEBUG
				if (toggleDebugView)
				{
					world.DebugDraw();
				}

#endif
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
					std::cout << "FRAME TIME " << frameTime << ", SLACK " << frameSlack << "\n";
				}
#endif
			}
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
