#include "pch.h"
#include "ingame-prerendered.h"
#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo/cairo.h>
#include "xlations.h"
#include "settings.h"
#include "constants.h"
#include "sfx.h"
#include "controller-stuff.h"

/// <summary>
/// Static, some of them animated, user interface controls and
/// labels used in-game, on transition and loading screens, and
/// in some of the menues, dialogs, and other parts of the UI
/// 
/// The PreRender() function reads the controller interface.
/// The need to draw controller button tips is determined from
/// whether or not any controller is connected at that moment.
/// </summary>
namespace blooDot::InGamePreRendered
{
	using namespace blooDot::ControllerStuff;

	SDL_Texture* PreRendered = nullptr;

	bool PreRender(SDL_Renderer* renderer)
	{
		if (PreRendered != nullptr)
		{
			Teardown();
		}

		/* 1. allocate a surface of reasonable size */
		PreRendered = NewTexture(renderer, GodsPreferredWidth, GodsPreferredHight, true, true);
		if (PreRendered == nullptr)
		{
			return false;
		}
		
		/* 2. draw all that we need onto there */
		_DrawSkipButton();

		return true;
	}

	bool _DrawSkipButton()
	{
		const auto& sink = BeginTextureDrawing(PreRendered, GodsPreferredWidth, GodsPreferredHight);
		const auto drawControllerTips = IsAnyControllerConnected();

		SDL_Rect buttonFrame = {
			static_cast<int>(InGameControlMargin),
			static_cast<int>(InGameControlMargin),
			static_cast<int>(InGameSkipButtonWidth),
			static_cast<int>(InGameButtonHeight),
		};

		/* 1. the unpressed, enabled state */
		DrawButton(
			sink,
			buttonFrame.x,
			buttonFrame.y,
			buttonFrame.w,
			buttonFrame.h,
			ControlHighlight::CH_INGAME
		);

		/* the controller (X) button tip */
		SDL_Rect controllerTipFrame = {
			0,
			1, // empirical: controller button tips are all-caps
			2 * static_cast<int>(CONTROLLER_BUTTON_TIP_RADIUS),
			2 * static_cast<int>(CONTROLLER_BUTTON_TIP_RADIUS)
		};

		if (drawControllerTips)
		{
			DrawControllerButtonTip(
				sink,
				buttonFrame.x,
				buttonFrame.y,
				ControlHighlight::CH_INGAME
			);
		}

		EndTextureDrawing(PreRendered, sink);

		/* at last, the labels (these do their own texture locking) */
		if (!DrawText(
			PreRendered,
			&buttonFrame,
			FONT_KEY_DIALOG_FAT,
			IN_GAME_BUTTON_LABEL_FONT_SIZE,
			literalMenuSkip,
			ButtonTextColor
		))
		{
			return false;
		}

		if (drawControllerTips)
		{
			if (!DrawText(
				PreRendered,
				&controllerTipFrame,
				FONT_KEY_DIALOG,
				CONTROLLER_BUTTON_TIP_FONT_SIZE,
				CONTROLLER_BUTTON_TIP_X,
				ButtonTextColor
			))
			{
				return false;
			}
		}

		return true;
	}

	void Teardown()
	{
		DestroyTexture(&PreRendered);
	}
}