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

/// <summary>
/// Static, some of them animated, user interface controls and
/// labels used in-game, on transition and loading screens, and
/// in some of the menues, dialogs, and other parts of the UI
/// </summary>
namespace blooDot::InGamePreRendered
{
	SDL_Texture* PreRendered = nullptr;

	bool PreRender(SDL_Renderer* renderer)
	{
		if (PreRendered != nullptr)
		{
			Teardown();
		}

		/* 1. allocate a surface of reasonable size */
		PreRendered = NewTexture(renderer, 640, 480, true, true);
		if (PreRendered == nullptr)
		{
			return false;
		}
		
		const auto& sink = BeginTextureDrawing(PreRendered, 640, 480);

		/* 2. draw all that we need onto there */
		SDL_Rect buttonFrame = {
			static_cast<int>(InGameControlMargin),
			static_cast<int>(InGameControlMargin),
			static_cast<int>(InGameSkipButtonWidth),
			static_cast<int>(InGameButtonHeight),
		};

		DrawButton(
			sink,
			buttonFrame.x,
			buttonFrame.y,
			buttonFrame.w,
			buttonFrame.h,
			ControlHighlight::CH_INGAME
		);

		EndTextureDrawing(PreRendered, sink);
		if (!DrawText(
			PreRendered,
			&buttonFrame,
			FONT_KEY_DIALOG_FAT,
			21,
			literalMenuSkip,
			ButtonTextColor
		))
		{
			return false;
		}

		return true;
	}

	void Teardown()
	{
		DestroyTexture(&PreRendered);
	}
}