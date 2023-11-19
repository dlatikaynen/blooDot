#pragma once
#include "enums.h"

typedef struct MenuDialogInteractionStruct {
	int selectedItemIndex = -1;
	int itemCount = 0;
	bool enterMenuItem = false;
	bool leaveDialog = false;
	bool leaveMain = false;
	int carouselItemIndex = -1;
	int carouselCount = 0;
	int carouselSelectedIndex = 0;
	int carouselMoveTo = 0;
	DialogMenuResult dialogResult = DialogMenuResult::DMR_NONE;
} MenuDialogInteraction;

namespace blooDot::MenuCommon
{
	void HandleMenu(MenuDialogInteraction* interactionState);
	void PrepareIconRect(SDL_Rect* rect, int vignetteIndex, int vignetteWidth, int bounceMargin);
	void DrawIcon(SDL_Renderer* renderer, int chunkKey, int vignetteIndex, int vignetteWidth, int bounceMargin);
}