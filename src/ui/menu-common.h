#ifndef MENU_COMMON_H
#define MENU_COMMON_H

#include "../shared-constants.h"
#include "SDL3/SDL.h"

namespace blooDot::MenuCommon
{
    typedef struct MenuDialogInteractionStruct {
        bool isTopLevel = false;
        int selectedItemIndex = -1;
        int itemCount = 0;
        bool enterMenuItem = false;
        bool leaveDialog = false;
        bool leaveMain = false;
        bool abortMain = false;
        int carouselItemIndex = -1;
        int carouselCount = 0;
        int carouselSelectedIndex = 0;
        int carouselMoveTo = 0;
        Constants::DialogMenuResult dialogResult = Constants::DialogMenuResult::DMR_NONE;
    } MenuDialogInteraction;

    void HandleMenu(MenuDialogInteraction* interactionState);
    bool DrawMenuPanel(SDL_Renderer* renderer, const SDL_FRect* rect, SDL_Texture* caption, SDL_FRect* captionRect);
    void PrepareIconRect(SDL_Rect* rect, int vignetteIndex, int vignetteWidth, int bounceMargin);
    void DrawIcon(SDL_Renderer* renderer, int chunkKey, int vignetteIndex, int vignetteWidth, int bounceMargin);
}
#endif //MENU_COMMON_H
