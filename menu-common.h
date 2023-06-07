#pragma once

typedef struct MenuDialogInteractionStruct {
	int selectedItemIndex = -1;
	int itemCount = 0;
	bool enterMenuItem = false;
	bool leaveDialog = false;
	bool leaveMain = false;
} MenuDialogInteraction;

namespace blooDot::MenuCommon
{
	void HandleMenu(MenuDialogInteraction* interactionState);
	void PrepareIconRect(SDL_Rect* rect, int vignetteIndex, int vignetteWidth, int bounceMargin);
	void DrawIcon(SDL_Renderer* renderer, int chunkKey, int vignetteIndex, int vignetteWidth, int bounceMargin);
}