#include "pch.h"
#include <SDL2/SDL.h>
#include "menu-common.h"
#include "settings.h"
#include "sfx.h"
#include "resutil.h"
#include "geom2d.h"

extern SettingsStruct Settings;

using namespace blooDot::Geometry2d;
namespace blooDot::MenuCommon
{
	SDL_Event dialogEvent;

	void HandleMenu(MenuDialogInteraction* interactionState)
	{
		if (interactionState->leaveDialog)
		{
			interactionState->leaveDialog = false;
		}

		if (interactionState->leaveMain)
		{
			interactionState->leaveMain = false;
		}

		if (interactionState->enterMenuItem)
		{
			interactionState->enterMenuItem = false;
		}

		while (SDL_PollEvent(&dialogEvent) != 0)
		{
			switch (dialogEvent.type)
			{
			case SDL_QUIT:
				interactionState->leaveDialog = true;
				interactionState->leaveMain = true;
				break;

			case SDL_KEYDOWN:
				switch (dialogEvent.key.keysym.scancode)
				{
				case SDL_SCANCODE_UP:
				case SDL_SCANCODE_KP_8:
				case SDL_SCANCODE_W:
				UP:
					if (interactionState->selectedItemIndex == 0)
					{
						blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
					}
					else
					{
						--interactionState->selectedItemIndex;
						blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
					}

					break;

				case SDL_SCANCODE_DOWN:
				case SDL_SCANCODE_KP_2:
				case SDL_SCANCODE_S:
				DOWN:
					if (interactionState->selectedItemIndex == (interactionState->itemCount - 1))
					{
						blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
					}
					else
					{
						++interactionState->selectedItemIndex;
						blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
					}

					break;

				case SDL_SCANCODE_PAGEDOWN:
				case SDL_SCANCODE_END:
					if (interactionState->selectedItemIndex == (interactionState->itemCount - 1))
					{
						blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
					}
					else
					{
						interactionState->selectedItemIndex = interactionState->itemCount - 1;
						blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
					}

					break;

				case SDL_SCANCODE_PAGEUP:
					if (interactionState->selectedItemIndex == 0)
					{
						blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
					}
					else
					{
						interactionState->selectedItemIndex = 0;
						blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
					}

					break;

				case SDL_SCANCODE_HOME:
				{
					auto const& destSelection = ::Settings.CurrentSavegameIndex == 0 ? MMI_NEWSINGLE : MMI_CUE;
					if (interactionState->selectedItemIndex == destSelection)
					{
						blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
					}
					else
					{
						interactionState->selectedItemIndex = destSelection;
						blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
					}

					break;
				}

				case SDL_SCANCODE_LEFT:
				case SDL_SCANCODE_KP_4:
				case SDL_SCANCODE_A:
					if (interactionState->carouselItemIndex == -1)
					{
						break;
					}

					if (interactionState->selectedItemIndex != interactionState->carouselItemIndex)
					{
						interactionState->selectedItemIndex = interactionState->carouselItemIndex;
					}

					if (interactionState->carouselSelectedIndex == 0)
					{
						blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
					}
					else if (interactionState->carouselMoveTo == interactionState->carouselSelectedIndex)
					{
						interactionState->carouselMoveTo = interactionState->carouselSelectedIndex - 1;
						blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
					}
					else if (interactionState->carouselMoveTo != 0)
					{
						--interactionState->carouselMoveTo;
					}

					break;

				case SDL_SCANCODE_RIGHT:
				case SDL_SCANCODE_KP_6:
				case SDL_SCANCODE_D:
					if (interactionState->carouselItemIndex == -1)
					{
						break;
					}

					if (interactionState->selectedItemIndex != interactionState->carouselItemIndex)
					{
						interactionState->selectedItemIndex = interactionState->carouselItemIndex;
					}

					if (interactionState->carouselSelectedIndex == interactionState->carouselCount - 1)
					{
						blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
					}
					else if (interactionState->carouselMoveTo == interactionState->carouselSelectedIndex)
					{
						interactionState->carouselMoveTo = interactionState->carouselSelectedIndex + 1;
						blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
					}
					else if (interactionState->carouselMoveTo != interactionState->carouselCount - 1)
					{
						++interactionState->carouselMoveTo;
					}

					break;

				case SDL_SCANCODE_RETURN:
				case SDL_SCANCODE_RETURN2:
				case SDL_SCANCODE_KP_ENTER:
				case SDL_SCANCODE_SPACE:
				A:
					blooDot::Sfx::Play(SoundEffect::SFX_SELCONF);
					interactionState->enterMenuItem = true;
					break;

				case SDL_SCANCODE_ESCAPE:
				B:
					interactionState->leaveDialog = true;
					break;
				}

				break;

			case SDL_CONTROLLERBUTTONDOWN:
				switch (dialogEvent.cbutton.button)
				{
				case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN:
				case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B:
					goto DOWN;
				case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP:
				case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X:
					goto UP;
				case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A:
					goto A;
				case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y:
					goto B;
				}

			case SDL_CONTROLLERAXISMOTION:
				if (dialogEvent.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
				{
					//srcRect.x += dialogEvent.caxis.value / 8192;
				}

				if (dialogEvent.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
				{
					//srcRect.y += dialogEvent.caxis.value / 8192;
				}

				break;
			}
		}
	}

	void PrepareIconRect(SDL_Rect* rect, int vignetteIndex, int vignetteWidth, int bounceMargin)
	{
		*rect = { bounceMargin + 30 + vignetteIndex * vignetteWidth, 102, vignetteWidth - 60, 64 };
	}

	void DrawIcon(SDL_Renderer* renderer, int chunkKey, int vignetteIndex, int vignetteWidth, int bounceMargin)
	{
		SDL_Rect iconRect;
		SDL_Rect centerRect;
		SDL_Rect destRect;
		auto icon = blooDot::Res::LoadPicture(renderer, chunkKey, &iconRect);

		PrepareIconRect(&destRect, vignetteIndex, vignetteWidth, bounceMargin);
		CenterRectInRect(&destRect, &iconRect, &centerRect);
		SDL_RenderCopy(renderer, icon, &iconRect, &centerRect);
		SDL_DestroyTexture(icon);
	}
}