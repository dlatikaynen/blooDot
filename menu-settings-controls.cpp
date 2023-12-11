#include "pch.h"
#include "menu-settings-controls.h"

#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo/cairo.h>
#include "xlations.h"
#include "settings.h"
#include "constants.h"
#include "sfx.h"
#include "menu-common.h"

constexpr int const bounceMargin = 10;
constexpr int const vignetteWidth = 480;
constexpr int const vignetteHeight = 220;
constexpr int const vignetteGap = 10;
constexpr int const vignetteCount = InputDeviceChoices::IDC_CONTROLLER4 + 1;

extern SettingsStruct Settings;
extern bool mainRunning;

namespace blooDot::MenuSettingsControls
{
	SDL_Event ctrlsSettingsMenuEvent;
	bool ctrlsSettingsMenuRunning = false;
	auto menuSelection = ControlsSettingsMenuItems::CSMI_CANCEL;
	auto selectedDevice = InputDeviceChoices::IDC_KYBD;
	auto movingToDevice = InputDeviceChoices::IDC_KYBD;
	SDL_Texture* slidingDevices;
	int sliderTextureWidth;
	int sliderOffsetLeft = bounceMargin;
	int slideSpeed = 0;
	int targetOffsetLeft = 0;

	SDL_JoystickID controller1Connected = -1;
	SDL_JoystickID controller2Connected = -1;
	SDL_JoystickID controller3Connected = -1;
	SDL_JoystickID controller4Connected = -1;

	bool ControlsSettingsMenuLoop(SDL_Renderer* renderer)
	{
		constexpr int const startY = 94;
		constexpr int const stride = 46;
		constexpr int backGap = stride / 2;
		constexpr int const sliderY = startY + stride + backGap;
		constexpr SDL_Rect const carouselDestRect = { 80,sliderY,vignetteWidth,vignetteHeight };

		_PrepareControls(renderer);
		ctrlsSettingsMenuRunning = true;
		sliderOffsetLeft = static_cast<int>(selectedDevice) * vignetteWidth + bounceMargin;

		SDL_Rect outerMenuRect{ 50,45,540,390 };
		SDL_Rect titleRect{ 0,0,0,0 };
		SDL_Rect cancelRect{ 0,0,0,0 };
		SDL_Rect carouselSrcRect = { sliderOffsetLeft,0,vignetteWidth,vignetteHeight };

		const auto titleTexture = RenderText(
			renderer,
			&titleRect,
			FONT_KEY_ALIEN,
			26,
			literalAlienCtrlSettingsMenuLabel,
			AlienTextColor
		);

		/* menu item text */
		const auto cancelTexture = RenderText(
			renderer,
			&cancelRect,
			FONT_KEY_DIALOG,
			23,
			literalMenuCancel,
			ButtonTextColor
		);

		_StatusQuo();

		unsigned short frame = 0L;
		while (ctrlsSettingsMenuRunning)
		{
			while (SDL_PollEvent(&ctrlsSettingsMenuEvent) != 0)
			{
				switch (ctrlsSettingsMenuEvent.type)
				{
				case SDL_CONTROLLERDEVICEADDED:
				{
					auto instanceId = SDL_JoystickGetDeviceInstanceID(ctrlsSettingsMenuEvent.cdevice.which);

					if (instanceId < 0)
					{
						break;
					}

					auto playerIndex = SDL_JoystickGetDevicePlayerIndex(instanceId);
					
					if (playerIndex <= 0)
					{
						if (controller1Connected == -1)
						{
							controller1Connected = instanceId;
						}
						else if (controller2Connected == -1)
						{
							controller2Connected = instanceId;
						}
						else if (controller3Connected == -1)
						{
							controller3Connected = instanceId;
						}
						else if (controller4Connected == -1)
						{
							controller4Connected = instanceId;
						}
					}
					else if (playerIndex == 1)
					{
						controller1Connected = instanceId;
					}
					else if (playerIndex == 2)
					{
						controller2Connected = instanceId;
					}
					else if (playerIndex == 3)
					{
						controller3Connected = instanceId;
					}
					else if (playerIndex == 4)
					{
						controller4Connected = instanceId;
					}

					break;
				}

				case SDL_CONTROLLERDEVICEREMOVED:
				{
					auto instanceId = ctrlsSettingsMenuEvent.cdevice.which;

					if (controller1Connected == instanceId)
					{
						controller1Connected = -1;
					}

					if (controller2Connected == instanceId)
					{
						controller2Connected = -1;
					}

					if (controller3Connected == instanceId)
					{
						controller3Connected = -1;
					}

					if (controller4Connected == instanceId)
					{
						controller4Connected = -1;
					}

					break;
				}

				case SDL_QUIT:
					mainRunning = false;
					ctrlsSettingsMenuRunning = false;
					break;

				case SDL_KEYDOWN:
					switch (ctrlsSettingsMenuEvent.key.keysym.scancode)
					{
					case SDL_SCANCODE_DOWN:
					case SDL_SCANCODE_KP_2:
					case SDL_SCANCODE_S:
					case SDL_SCANCODE_PAGEDOWN:
					case SDL_SCANCODE_END:
						if (menuSelection == CSMI_INPUT_DEVICES)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = CSMI_INPUT_DEVICES;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_UP:
					case SDL_SCANCODE_KP_8:
					case SDL_SCANCODE_W:
					case SDL_SCANCODE_PAGEUP:
					case SDL_SCANCODE_HOME:
						if (menuSelection == CSMI_CANCEL)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = CSMI_CANCEL;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_LEFT:
					case SDL_SCANCODE_KP_4:
					case SDL_SCANCODE_A:
						if (menuSelection == CSMI_CANCEL)
						{
							menuSelection = CSMI_INPUT_DEVICES;
						}

						if (selectedDevice == InputDeviceChoices::IDC_KYBD)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else if (movingToDevice == selectedDevice)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
							movingToDevice = static_cast<InputDeviceChoices>(static_cast<int>(selectedDevice) - 1);
						}
						else if (movingToDevice != InputDeviceChoices::IDC_KYBD)
						{
							--movingToDevice;
						}

						break;

					case SDL_SCANCODE_RIGHT:
					case SDL_SCANCODE_KP_6:
					case SDL_SCANCODE_D:
						if (menuSelection == CSMI_CANCEL)
						{
							menuSelection = CSMI_INPUT_DEVICES;
						}

						if (selectedDevice == InputDeviceChoices::IDC_CONTROLLER4)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else if (movingToDevice == selectedDevice)
						{
							movingToDevice = static_cast<InputDeviceChoices>(static_cast<int>(selectedDevice) + 1);
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}
						else if (movingToDevice != InputDeviceChoices::IDC_CONTROLLER4)
						{
							++movingToDevice;
						}

						break;

					case SDL_SCANCODE_RETURN:
					case SDL_SCANCODE_RETURN2:
					case SDL_SCANCODE_KP_ENTER:
					case SDL_SCANCODE_SPACE:
						if (menuSelection == CSMI_CANCEL)
						{
							ctrlsSettingsMenuRunning = false;
						}

						blooDot::Sfx::Play(SoundEffect::SFX_SELCONF);
						break;

					case SDL_SCANCODE_ESCAPE:
						ctrlsSettingsMenuRunning = false;
						break;
					}

					break;
				}
			}

			if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = IMG_GetError();
				ReportError("Failed to clear the controls settings menu screen", clearError);
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
			SDL_RenderFillRect(renderer, &outerMenuRect);
			SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
			if (SDL_RenderDrawRect(renderer, &outerMenuRect) < 0)
			{
				const auto drawRectError = SDL_GetError();
				ReportError("Failed to draw controls settings menu panel border", drawRectError);
				ctrlsSettingsMenuRunning = false;
			};

			DrawLabel(renderer, 286, 51, titleTexture, &titleRect);

			const auto drawingTexture = BeginRenderDrawing(renderer, 640, 480);
			if (drawingTexture)
			{
				auto const& drawingSink = GetDrawingSink();

				auto itemToDraw = CSMI_CANCEL;
				for (auto y = startY; y < 200; y += stride)
				{
					DrawButton(
						drawingSink,
						30 + 50 + 70,
						y + vignetteHeight - 8,
						55,
						44,
						itemToDraw == menuSelection && selectedDevice == InputDeviceChoices::IDC_KYBD ? CH_MAINMENU : CH_NONE
					);

					DrawButton(
						drawingSink,
						30 + 50 + 80 + 55 + 55 * 0 + 20,
						y + vignetteHeight - 8,
						44,
						44,
						itemToDraw == menuSelection && selectedDevice == InputDeviceChoices::IDC_CONTROLLER1 ? CH_MAINMENU : CH_NONE
					);

					if (controller1Connected != -1)
					{
						DrawActiveControllerSquare(
							drawingSink,
							30 + 50 + 80 + 55 + 55 * 0 + 20 + 10,
							y + vignetteHeight + 4,
							44 - (16 + 5),
							44 - (16 + 5)
						);
					}

					DrawButton(
						drawingSink,
						30 + 50 + 80 + 55 + 55 * 1 + 20,
						y + vignetteHeight - 8,
						44,
						44,
						itemToDraw == menuSelection && selectedDevice == InputDeviceChoices::IDC_CONTROLLER2 ? CH_MAINMENU : CH_NONE
					);

					if (controller2Connected != -1)
					{
						DrawActiveControllerSquare(
							drawingSink,
							30 + 50 + 80 + 55 + 55 * 1 + 20 + 10,
							y + vignetteHeight + 4,
							44 - (16 + 5),
							44 - (16 + 5)
						);
					}

					DrawButton(
						drawingSink,
						30 + 50 + 80 + 55 + 55 * 2 + 20,
						y + vignetteHeight - 8,
						44,
						44,
						itemToDraw == menuSelection && selectedDevice == InputDeviceChoices::IDC_CONTROLLER3 ? CH_MAINMENU : CH_NONE
					);

					if (controller3Connected != -1)
					{
						DrawActiveControllerSquare(
							drawingSink,
							30 + 50 + 80 + 55 + 55 * 2 + 20 + 10,
							y + vignetteHeight + 4,
							44 - (16 + 5),
							44 - (16 + 5)
						);
					}

					DrawButton(
						drawingSink,
						30 + 50 + 80 + 55 + 55 * 3 + 20,
						y + vignetteHeight - 8,
						44,
						44,
						itemToDraw == menuSelection && selectedDevice == InputDeviceChoices::IDC_CONTROLLER4 ? CH_MAINMENU : CH_NONE
					);

					if (controller4Connected != -1)
					{
						DrawActiveControllerSquare(
							drawingSink,
							30 + 50 + 80 + 55 + 55 * 3 + 20 + 10,
							y + vignetteHeight + 4,
							44 - (16 + 5),
							44 - (16 + 5)
						);
					}

					DrawButton(
						drawingSink,
						30 + 50,
						y,
						vignetteWidth,
						itemToDraw > CSMI_CANCEL ? vignetteHeight : 42,
						itemToDraw == menuSelection ? CH_MAINMENU : CH_NONE
					);

					if (itemToDraw == menuSelection)
					{
						if (menuSelection == CSMI_CANCEL)
						{
							DrawChevron(drawingSink, 80 - 7, y + 21, false, frame);
							DrawChevron(drawingSink, 80 + vignetteWidth + 7, y + 21, true, frame);
						}
						else
						{
							if (selectedDevice != ::IDC_KYBD)
							{
								DrawChevron(drawingSink, 80 - 9, y + 105, true, frame);
							}

							if (selectedDevice != ::IDC_CONTROLLER4)
							{
								DrawChevron(drawingSink, 80 + vignetteWidth + 9, y + 105, false, frame);
							}
						}
					}

					if (itemToDraw == CSMI_CANCEL)
					{
						y += backGap;
					}

					itemToDraw = static_cast<ControlsSettingsMenuItems>(itemToDraw + 1);
				}

				EndRenderDrawing(renderer, drawingTexture, nullptr);

				DrawLabel(renderer, 235, startY + 0 * stride + 0 * backGap, cancelTexture, &cancelRect);

				/* render the carousel choice (and the sliding animation) */
				_AnimateCarousel();
				carouselSrcRect.x = sliderOffsetLeft;
				SDL_RenderCopy(renderer, slidingDevices, &carouselSrcRect, &carouselDestRect);
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		_Teardown();
		titleTexture&& [titleTexture] { SDL_DestroyTexture(titleTexture); return false; }();
		cancelTexture&& [cancelTexture] { SDL_DestroyTexture(cancelTexture); return false; }();

		return ctrlsSettingsMenuRunning;
	}

	void _AnimateCarousel()
	{
		if (movingToDevice == selectedDevice)
		{
			slideSpeed = 0;
			return;
		}

		if (slideSpeed == 0)
		{
			targetOffsetLeft = bounceMargin + static_cast<int>(movingToDevice) * vignetteWidth;
			slideSpeed = 40;
		}

		auto direction = targetOffsetLeft < sliderOffsetLeft ? -1 : 1;
		sliderOffsetLeft += direction * slideSpeed;
		if (direction == 1)
		{
			if (sliderOffsetLeft >= targetOffsetLeft)
			{
				sliderOffsetLeft = targetOffsetLeft;
				selectedDevice = movingToDevice;
			}
		}
		else
		{
			if (sliderOffsetLeft <= targetOffsetLeft)
			{
				sliderOffsetLeft = targetOffsetLeft;
				selectedDevice = movingToDevice;
			}
		}
	}

	void _StatusQuo()
	{
		auto numJoysticks = SDL_NumJoysticks();

		for (auto i = 0; i < numJoysticks; ++i)
		{
			auto instanceId = SDL_JoystickGetDeviceInstanceID(i);

			if (instanceId >= 0)
			{
				auto playerIndex = SDL_JoystickGetDevicePlayerIndex(instanceId);

				if (playerIndex == 1)
				{
					controller1Connected = instanceId;
				}
				else if (playerIndex == 2)
				{
					controller2Connected = instanceId;
				}
				else if (playerIndex == 3)
				{
					controller3Connected = instanceId;
				}
				else if (playerIndex == 4)
				{
					controller4Connected = instanceId;
				}
			}
		}

		// controllers not identifying as players get the leftovers
		for (auto i = 0; i < numJoysticks; ++i)
		{
			auto instanceId = SDL_JoystickGetDeviceInstanceID(i);

			if (instanceId >= 0)
			{
				auto playerIndex = SDL_JoystickGetDevicePlayerIndex(instanceId);

				if (playerIndex == -1)
				{
					if (!controller1Connected)
					{
						controller1Connected = instanceId;
					}
					else if (!controller2Connected)
					{
						controller2Connected = instanceId;
					}
					else if (!controller3Connected)
					{
						controller3Connected = instanceId;
					}
					else if (!controller4Connected)
					{
						controller4Connected = instanceId;
					}
				}
			}
		}
	}

	void _PrepareControls(SDL_Renderer* renderer)
	{
		sliderTextureWidth = vignetteCount * vignetteWidth + (vignetteCount - 1) * vignetteGap + 2 * bounceMargin;
		slidingDevices = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET,
			sliderTextureWidth,
			vignetteHeight
		);

		if (slidingDevices)
		{
			if (SDL_SetTextureBlendMode(slidingDevices, SDL_BLENDMODE_BLEND) < 0)
			{
				const auto carouselBlendmodeError = SDL_GetError();
				ReportError("Could not set blend mode of sliding texture", carouselBlendmodeError);
				SDL_DestroyTexture(slidingDevices);
				slidingDevices = NULL;

				return;
			}

			if (SDL_SetRenderTarget(renderer, slidingDevices) < 0)
			{
				const auto targetError = SDL_GetError();
				ReportError("Could not set sliding texture as the render target", targetError);
				return;
			}

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			_VignetteLabel(renderer, FONT_KEY_DIALOG_FAT, 28, 0, 30, literalControlsKeyboard);
			_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 0, 190, literalAll);

			_VignetteLabel(renderer, FONT_KEY_DIALOG_FAT, 28, 1, 30, literalcontrollerLabel1);
			_VignetteLabel(renderer, FONT_KEY_ALIEN, 25, 1, 189, literalplayerName1);

			_VignetteLabel(renderer, FONT_KEY_DIALOG_FAT, 28, 2, 30, literalcontrollerLabel2);
			_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 2, 190, literalplayerName2);

			_VignetteLabel(renderer, FONT_KEY_DIALOG_FAT, 28, 3, 30, literalcontrollerLabel3);
			_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 3, 190, literalplayerName3);

			_VignetteLabel(renderer, FONT_KEY_DIALOG_FAT, 28, 4, 30, literalcontrollerLabel4);
			_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, 4, 190, literalplayerName4);

			if (SDL_SetRenderTarget(renderer, NULL) < 0)
			{
				const auto restoreError = SDL_GetError();
				ReportError("Could not restore render target after rendering to sliding texture", restoreError);
				return;
			}

			return;
		}

		const auto newCarouselError = SDL_GetError();
		ReportError("Could not allocate sliding texture", newCarouselError);
	}

	void _VignetteLabel(SDL_Renderer* renderer, int font, int size, int vignetteIndex, int y, const char* text)
	{
		SDL_Rect rectLabel = { 0,0,0,0 };
		const auto textureLabel = RenderText(
			renderer,
			&rectLabel,
			font,
			size,
			text,
			{ 250, 230, 230, 245 }
		);

		CenterLabel(
			renderer,
			bounceMargin + vignetteIndex * vignetteWidth + vignetteWidth / 2,
			y,
			textureLabel,
			&rectLabel
		);

		SDL_DestroyTexture(textureLabel);
	}

	void _Teardown()
	{
		DestroyTexture(&slidingDevices);
	}
}