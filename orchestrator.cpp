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
#include "geom2d.h"
#include "newworld.h"

using namespace blooDot::Dings;
using namespace blooDot::World;

Uint32 SDL_USEREVENT_SAVE = 0;
Uint32 SDL_USEREVENT_AUTOSAVE = 0;
Uint32 SDL_USEREVENT_LEAVE = 0; // [sic], they're only allocated later

extern SettingsStruct Settings;
extern bool isCreatorMode;
extern SDL_Renderer* GameViewRenderer;
extern char activePlayers;
extern MobState* player[4];

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
		toggleDebugView = isCreatorMode;

		if (!InitializeDings())
		{
			mainRunning = false;
		}
		
		if (mainRunning && !InitializeNewWorld(renderer))
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
			bodyDef.angularDamping = 12.9f; // this will reduce on ice
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
			const auto& frequencyMill = preMultiplied * 1000.f;
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
					float pImpulseX = .0f;
					float pImpulseY = .0f;

					// p1
					if (activePlayers & (1 << iP1))
					{
						if (keys[SDL_SCANCODE_A])
						{
							pImpulseX = -1.7f;
						}
						else if (keys[SDL_SCANCODE_D])
						{
							pImpulseX = 1.7f;
						}

						if (keys[SDL_SCANCODE_S])
						{
							pImpulseY = 1.7f;
						}
						else if (keys[SDL_SCANCODE_W])
						{
							pImpulseY = -1.7f;
						}

						_InitiatePlayerMovement(p1Body, pImpulseX, pImpulseY);
						pImpulseX = pImpulseY = 0;
					}

					// p2
					if (activePlayers & (1 << iP2))
					{
						if (keys[SDL_SCANCODE_F])
						{
							pImpulseX = -1.7f;
						}
						else if (keys[SDL_SCANCODE_H])
						{
							pImpulseX = 1.7f;
						}

						if (keys[SDL_SCANCODE_G])
						{
							pImpulseY = 1.7f;
						}
						else if (keys[SDL_SCANCODE_T])
						{
							pImpulseY = -1.7f;
						}

						_InitiatePlayerMovement(p2Body, pImpulseX, pImpulseY);
						pImpulseX = pImpulseY = 0;
					}

					// p3
					if (activePlayers & (1 << iP3))
					{
						if (keys[SDL_SCANCODE_J])
						{
							pImpulseX = -1.7f;
						}
						else if (keys[SDL_SCANCODE_L])
						{
							pImpulseX = 1.7f;
						}

						if (keys[SDL_SCANCODE_K])
						{
							pImpulseY = 1.7f;
						}
						else if (keys[SDL_SCANCODE_I])
						{
							pImpulseY = -1.7f;
						}

						_InitiatePlayerMovement(p3Body, pImpulseX, pImpulseY);
						pImpulseX = pImpulseY = 0;
					}

					// p4
					if (activePlayers & (1 << iP4))
					{
						if (keys[SDL_SCANCODE_SEMICOLON])
						{
							pImpulseX = -1.7f;
						}
						else if (keys[SDL_SCANCODE_BACKSLASH])
						{
							pImpulseX = 1.7f;
						}

						if (keys[SDL_SCANCODE_APOSTROPHE])
						{
							pImpulseY = 1.7f;
						}
						else if (keys[SDL_SCANCODE_LEFTBRACKET])
						{
							pImpulseY = -1.7f;
						}

						_InitiatePlayerMovement(p4Body, pImpulseX, pImpulseY);
					}

					// viewport scrolling
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

				// rare legit use for a minge & bracket, guess why
				{
					/* compute one physics step and set the positions
					 * and orientations for the next frame to rrender */
					world.Step(timeStep, velocityIterations, positionIterations);

					if (activePlayers & (1 << iP1))
					{
						SetPlayerPosition(p1Body, player[iP1]);
					}

					if (activePlayers & (1 << iP2))
					{
						SetPlayerPosition(p2Body, player[iP2]);
					}

					if (activePlayers & (1 << iP3))
					{
						SetPlayerPosition(p3Body, player[iP3]);
					}

					if (activePlayers & (1 << iP4))
					{
						SetPlayerPosition(p4Body, player[iP4]);
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
				const auto& frameTime = (frameEnded - frameStart) / frequencyMill;
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
		TeardownDings();
	}

	void _InitiatePlayerMovement(b2Body* body, float impulseX, float impulseY)
	{
		if (impulseX != 0 || impulseY != 0)
		{
			body->ApplyLinearImpulseToCenter({ impulseX, impulseY }, true);
			/* at this point, the new velocity is already imparted, so
			 * our linear velocity victor points to where we want to go.
			 * we compare it to our orientation victor:
			 * if the orientation victor is significantly different, we
			 * convert most of our intended linear motion into an angular
			 * movement to orient us towards the walking direction (as a
			 * biped would). if the orientation victor is only marginally
			 * different, we just veer a bit toward it, so it aligns perfectly
			 * almost immediately (except player 4, who always goes off on a
			 * bit of a tangent. speaking of which, caution, trigonomometrical
			 * algebraics ahead, sine and her cousins too. */
			const auto& newVelocitty = body->GetLinearVelocity();
			if (newVelocitty.LengthSquared() != 0)
			{
				const auto& currentAngle = body->GetAngle();
				const auto& towardsAngle = std::atan2f(-newVelocitty.x, newVelocitty.y);
				body->SetAngularVelocity(blooDot::Geometry2d::sgn(towardsAngle - currentAngle) * 6.8f);
				const auto& magnitude = std::fabsf(towardsAngle - currentAngle);
				if (magnitude > .5f)
				{
					// stop in their tracks when much rotation is needed first
					body->ApplyLinearImpulseToCenter(
					{
						-impulseX,
						-impulseY
					}, true);
				}
				else if (magnitude > .15f)
				{
					// hefty, but tresholded rotation to orient towards direction of movement
					body->ApplyLinearImpulseToCenter(
					{
						-impulseX * magnitude / static_cast<float>(M_PI),
						-impulseY * magnitude / static_cast<float>(M_PI)
					}, true);
				}
			}
		}
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
