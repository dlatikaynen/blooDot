#include <SDL3/SDL.h>
#include "menu-common.h"

#include "dialog-controls.h"
#include "layout-constants.h"
#include "../../main.h"
#include "../../src/state/settings.h"
#include "../../src/snd/sfx.h"
#include "../../src/util/resutil.h"
#include "../../src/util/geom2d.h"

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
			case SDL_EVENT_QUIT:
				interactionState->leaveDialog = true;
				interactionState->leaveMain = true;
				break;

			case SDL_EVENT_KEY_DOWN:
				switch (dialogEvent.key.scancode)
				{
				case SDL_SCANCODE_UP:
				case SDL_SCANCODE_KP_8:
				case SDL_SCANCODE_W:
				UP:
					if (interactionState->selectedItemIndex == 0)
					{
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
					}
					else
					{
						--interactionState->selectedItemIndex;
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
					}

					break;

				case SDL_SCANCODE_DOWN:
				case SDL_SCANCODE_KP_2:
				case SDL_SCANCODE_S:
				DOWN:
					if (interactionState->selectedItemIndex == (interactionState->itemCount - 1))
					{
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
					}
					else
					{
						++interactionState->selectedItemIndex;
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
					}

					break;

				case SDL_SCANCODE_PAGEDOWN:
				case SDL_SCANCODE_END:
					if (interactionState->selectedItemIndex == (interactionState->itemCount - 1))
					{
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
					}
					else
					{
						interactionState->selectedItemIndex = interactionState->itemCount - 1;
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
					}

					break;

				case SDL_SCANCODE_PAGEUP:
					if (interactionState->selectedItemIndex == 0)
					{
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
					}
					else
					{
						interactionState->selectedItemIndex = 0;
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
					}

					break;

				case SDL_SCANCODE_HOME:
				{
					auto const& destSelection = Settings::SettingsData.CurrentSavegameIndex == 0
						? Constants::MMI_NEWSINGLE
						: Constants::MMI_CUE;

					if (interactionState->selectedItemIndex == destSelection)
					{
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
					}
					else
					{
						interactionState->selectedItemIndex = destSelection;
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
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
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
					}
					else if (interactionState->carouselMoveTo == interactionState->carouselSelectedIndex)
					{
						interactionState->carouselMoveTo = interactionState->carouselSelectedIndex - 1;
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
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
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_ASTERISK);
					}
					else if (interactionState->carouselMoveTo == interactionState->carouselSelectedIndex)
					{
						interactionState->carouselMoveTo = interactionState->carouselSelectedIndex + 1;
						blooDot::Sfx::Play(Sfx::SoundEffect::SFX_SELCHG);
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
					blooDot::Sfx::Play(Sfx::SoundEffect::SFX_SELCONF);
					interactionState->enterMenuItem = true;
					break;

				case SDL_SCANCODE_ESCAPE:
				B:
					interactionState->leaveDialog = true;
					break;

				default:
					break;
				}

				break;
			case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
				switch (dialogEvent.button.button)
				{
				case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
				case SDL_GAMEPAD_BUTTON_EAST:
					goto DOWN;
				case SDL_GAMEPAD_BUTTON_DPAD_UP:
				case SDL_GAMEPAD_BUTTON_WEST:
					goto UP;
				case SDL_GAMEPAD_BUTTON_SOUTH:
					goto A;
				case SDL_GAMEPAD_BUTTON_NORTH:
					goto B;
				default:
					break;
				}

				break;

				case SDL_EVENT_GAMEPAD_AXIS_MOTION:
				if (dialogEvent.gaxis.axis == SDL_GAMEPAD_AXIS_RIGHTX)
				{
					//srcRect.x += dialogEvent.gaxis.value / 8192;
				}

				if (dialogEvent.gaxis.axis == SDL_GAMEPAD_AXIS_RIGHTY)
				{
					//srcRect.y += dialogEvent.gaxis.value / 8192;
				}

				break;

			default:
				break;
			}
		}
	}

	bool DrawMenuPanel(SDL_Renderer* renderer, const SDL_FRect* rect, SDL_Texture* const caption, SDL_FRect* const captionRect) {
		SDL_SetRenderDrawColor(renderer, Ui::PanelFill.r, Ui::PanelFill.g, Ui::PanelFill.b, Ui::PanelFill.a);
		SDL_RenderFillRect(renderer, rect);
		SDL_SetRenderDrawColor(renderer, Ui::PanelStroke.r, Ui::PanelStroke.g, Ui::PanelStroke.b, Ui::PanelStroke.a);
		if (!SDL_RenderRect(renderer, rect))
		{
			const auto drawRectError = SDL_GetError();

			ReportError("Failed to draw panel border", drawRectError);

			return false;
		};

		DialogControls::CenterLabel(
			renderer,
			static_cast<int>(rect->x + rect->w / 2),
			static_cast<int>(rect->y + 6 + captionRect->h / 2),
			caption,
			captionRect
		);

		return true;
	}

	void PrepareIconRect(SDL_FRect* rect, const int vignetteIndex, const int vignetteWidth, const int bounceMargin)
	{
		*rect = {
			static_cast<float>(bounceMargin + 30 + vignetteIndex * vignetteWidth),
			102,
			static_cast<float>(vignetteWidth - 60),
			64
		};
	}

	void DrawIcon(SDL_Renderer* renderer, const int chunkKey, const int vignetteIndex, const int vignetteWidth, const int bounceMargin)
	{
		SDL_FRect iconRect;
		SDL_FRect centerRect;
		SDL_FRect destRect;
		auto icon = Res::LoadPicture(renderer, chunkKey, &iconRect);

		PrepareIconRect(&destRect, vignetteIndex, vignetteWidth, bounceMargin);
		Geometry2d::CenterRectInRect(&destRect, &iconRect, &centerRect);
		SDL_RenderTexture(renderer, icon, &iconRect, &centerRect);
		SDL_DestroyTexture(icon);
	}
}