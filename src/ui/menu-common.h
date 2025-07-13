#ifndef MENU_COMMON_H
#define MENU_COMMON_H

#include "../shared-constants.h"
#include "SDL3/SDL.h"

namespace blooDot::MenuCommon
{
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
        Constants::DialogMenuResult dialogResult = Constants::DialogMenuResult::DMR_NONE;
    } MenuDialogInteraction;

    void HandleMenu(MenuDialogInteraction* interactionState);
    void PrepareIconRect(SDL_Rect* rect, int vignetteIndex, int vignetteWidth, int bounceMargin);
    void DrawIcon(SDL_Renderer* renderer, int chunkKey, int vignetteIndex, int vignetteWidth, int bounceMargin);
}
#endif //MENU_COMMON_H
