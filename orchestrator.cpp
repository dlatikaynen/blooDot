#include "pch.h"
#include "orchestrator.h"
#include "menu-ingame.h"
#include "settings.h"
#include "savegame.h"
#include "constants.h"
#include "physics-render-binding.h"

#ifndef NDEBUG
#include "physicsdebugdraw.h"
#endif

Uint32 SDL_USEREVENT_SAVE = 0;
Uint32 SDL_USEREVENT_AUTOSAVE = 0;
Uint32 SDL_USEREVENT_LEAVE = 0; // [sic], they're only allocated later

extern SettingsStruct Settings;
extern SDL_Renderer* GameViewRenderer;
extern char activePlayers;

bool mainRunning = true;
#ifndef NDEBUG
bool toggleDebugView = false;
#endif

namespace blooDot::Orchestrator
{
	SDL_Event mainEvent;
	b2World world({ 0,0 });
	b2BodyDef bodyDef;
	b2CircleShape pShape;
	b2FixtureDef fixtureDef;
	float timeStep = MillisecondsPerFrame / 1000.f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;
	SDL_GameController* controller = nullptr;

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

			const auto& p1 = Player::GetState(iP1);
			const auto& p2 = Player::GetState(iP2);
			const auto& p3 = Player::GetState(iP3);
			const auto& p4 = Player::GetState(iP4);

			bodyDef.type = b2_dynamicBody;
			bodyDef.linearDamping = 6.68f;
			bodyDef.position.Set(
				(p1->Offset.x + GRIDUNIT / 2.f) / static_cast<float>(GRIDUNIT),
				(p2->Offset.y + GRIDUNIT / 2.f) / static_cast<float>(GRIDUNIT)
			);

			const auto& p1Body = world.CreateBody(&bodyDef);
			bodyDef.position.Set(
				(p2->Offset.x + GRIDUNIT / 2.f) / static_cast<float>(GRIDUNIT),
				(p2->Offset.y + GRIDUNIT / 2.f) / static_cast<float>(GRIDUNIT)
			);

			const auto& p2Body = world.CreateBody(&bodyDef);
			bodyDef.position.Set(
				(p3->Offset.x + GRIDUNIT / 2.f) / static_cast<float>(GRIDUNIT),
				(p3->Offset.y + GRIDUNIT / 2.f) / static_cast<float>(GRIDUNIT)
			);

			const auto& p3Body = world.CreateBody(&bodyDef);
			bodyDef.position.Set(
				(p4->Offset.x + GRIDUNIT / 2.f) / static_cast<float>(GRIDUNIT),
				(p4->Offset.y + GRIDUNIT / 2.f) / static_cast<float>(GRIDUNIT)
			);
			
			const auto& p4Body = world.CreateBody(&bodyDef);

			pShape.m_radius = .33f;
			fixtureDef.shape = &pShape;
			fixtureDef.density = 1.6f * static_cast<float>(M_PI);
			fixtureDef.friction = 0.3f;
			fixtureDef.restitution = .27f;
			p1Body->CreateFixture(&fixtureDef);
			p2Body->CreateFixture(&fixtureDef);
			p3Body->CreateFixture(&fixtureDef);
			p4Body->CreateFixture(&fixtureDef);
			
			/* wall physics */
			blooDot::Physics::AttachWorldPhysics(&world);

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
			SDL_GameControllerEventState(SDL_ENABLE);
			for (int i = 0; i < SDL_NumJoysticks(); ++i) 
			{
				if (SDL_IsGameController(i))
				{
					controller = SDL_GameControllerOpen(i);
					break;
				}
			}

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
					case SDL_CONTROLLERDEVICEADDED:
					{
						std::cout << "DEVICEADDED\n";
					}

					case SDL_CONTROLLERDEVICEREMOVED:
					{
						std::cout << "DEVICEREMOVED\n";
					}

					break;

					case SDL_CONTROLLERAXISMOTION:
					{
						const SDL_JoystickID joystickID = mainEvent.caxis.which;
						const SDL_GameControllerAxis axis = (SDL_GameControllerAxis)mainEvent.caxis.axis;
						const float value = mainEvent.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
						std::cout << "joystick " << joystickID << " on axis " << axis << " says " << value << "\n";						
					}

					break;

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
						p1Body->ApplyLinearImpulseToCenter({ -1.7f, 0 }, true);
					}
					else if (keys[SDL_SCANCODE_D])
					{
						p1Body->ApplyLinearImpulseToCenter({ 1.7f, 0 }, true);
					}

					if (keys[SDL_SCANCODE_S])
					{
						p1Body->ApplyLinearImpulseToCenter({ 0, 1.7f }, true);
					}
					else if (keys[SDL_SCANCODE_W])
					{
						p1Body->ApplyLinearImpulseToCenter({ 0, -1.7f }, true);
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
						p3Body->ApplyLinearImpulseToCenter({ -1.7f, 0 }, true);
					}

					if (keys[SDL_SCANCODE_L])
					{
						p3Body->ApplyLinearImpulseToCenter({ 1.7f, 0 }, true);
					}

					if (keys[SDL_SCANCODE_K])
					{
						p3Body->ApplyLinearImpulseToCenter({ 0, 1.7f }, true);
					}

					if (keys[SDL_SCANCODE_I])
					{
						p3Body->ApplyLinearImpulseToCenter({ 0, -1.7f }, true);
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
					/* compute one physics step and set the positions
					 * and rotations for the next frame to render */
					world.Step(timeStep, velocityIterations, positionIterations);

					if (activePlayers & 1)
					{
						const auto& pPosition = p1Body->GetPosition();
						const auto& pAngle = static_cast<int>(p1Body->GetAngle() * 180 / M_PI);
						SetPlayerPosition(
							iP1,
							(int)(pPosition.x* static_cast<float>(GRIDUNIT) - GRIDUNIT / 2.f),
							(int)(pPosition.y* static_cast<float>(GRIDUNIT) - GRIDUNIT / 2.f),
							pAngle
						);

						const auto velocity = p1Body->GetLinearVelocity().Length();
						if (velocity < 0.3f && velocity != 0)
						{
							p1Body->SetLinearVelocity({ 0,0 });
						}
					}

					if (activePlayers & 2)
					{
						const auto& pPosition = p2Body->GetPosition();
						const auto& pAngle = static_cast<int>(p2Body->GetAngle() * 180 / M_PI);
						SetPlayerPosition(
							iP2,
							(int)(pPosition.x * static_cast<float>(GRIDUNIT) - GRIDUNIT / 2.f),
							(int)(pPosition.y * static_cast<float>(GRIDUNIT) - GRIDUNIT / 2.f),
							pAngle
						);

						const auto velocity = p2Body->GetLinearVelocity().Length();
						if (velocity < 0.3f && velocity != 0)
						{
							p2Body->SetLinearVelocity({ 0,0 });
						}
					}

					if (activePlayers & 4)
					{
						const auto& pPosition = p3Body->GetPosition();
						const auto& pAngle = static_cast<int>(p3Body->GetAngle() * 180 / M_PI);
						SetPlayerPosition(
							iP3,
							(int)(pPosition.x* static_cast<float>(GRIDUNIT) - GRIDUNIT / 2.f),
							(int)(pPosition.y* static_cast<float>(GRIDUNIT) - GRIDUNIT / 2.f),
							pAngle
						);

						const auto velocity = p3Body->GetLinearVelocity().Length();
						if (velocity < 0.3f && velocity != 0)
						{
							p3Body->SetLinearVelocity({ 0,0 });
						}
					}

					if (activePlayers & 8)
					{
						const auto& pPosition = p4Body->GetPosition();
						const auto& pAngle = static_cast<int>(p4Body->GetAngle() * 180 / M_PI);
						SetPlayerPosition(
							iP4,
							(int)(pPosition.x * static_cast<float>(GRIDUNIT) - GRIDUNIT / 2.f),
							(int)(pPosition.y * static_cast<float>(GRIDUNIT) - GRIDUNIT / 2.f),
							pAngle
						);

						const auto velocity = p4Body->GetLinearVelocity().Length();
						if (velocity < 0.3f && velocity != 0)
						{
							p4Body->SetLinearVelocity({ 0,0 });
						}
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
		if (controller != nullptr)
		{
			SDL_GameControllerClose(controller);
		}

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
