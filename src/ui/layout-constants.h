#ifndef LAYOUT_CONSTANTS_H
#define LAYOUT_CONSTANTS_H
#include "SDL3/SDL_pixels.h"

namespace blooDot::Ui {
    /**
     * The fill color of menu panels
     */
    constexpr SDL_Color PanelFill = {0x40, 0x40, 0x40, 0xcf};

    /**
     * The border stroke color of menu panels
     */
    constexpr SDL_Color PanelStroke = {0xc5, 0xc5, 0xc5, 0xe9};

    /**
     * Almost white text caption color
     */
    constexpr SDL_Color CaptionColor = { 250,200,200,222 };

    /**
     * Font color of menu item captions
     */
    constexpr SDL_Color MenuItemCaptionColor = { 250, 230, 230, 245 } ;

    /**
     * Font size in points of a menu item caption text
     */
    constexpr int MenuItemCaptionFontSize = 23;

    /**
     * Font size for small static labels (tips, descriptions, annotations)
     */
    constexpr int AnnotationLabelFontSize = 13;

    /**
     * Font size in points of a menu panel title caption
     */
    constexpr int MenuPanelTitleFontSize = 26;

    /**
     * The initial delay in milliseconds of the splash background bounce
     */
    constexpr int BackgroundAnimInitialDelay = 237;

    /**
     * The average delay in milliseconds of the splash background bounce
     */
    constexpr int BackgroundAnimDelay = 101;
}

#endif //LAYOUT_CONSTANTS_H
