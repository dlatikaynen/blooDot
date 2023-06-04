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
}