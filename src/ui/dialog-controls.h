#ifndef DIALOG_CONTROLS_H
#define DIALOG_CONTROLS_H
#include "../../src/shared-constants.h"
#include <SDL3/SDL.h>
#include "../../cairo/include/cairo.h"
#include <cmath>

namespace blooDot::DialogControls {
    constexpr auto HALFPI = M_PI / 2.0;
    constexpr auto BUTTON_INSET = 4.0;
    constexpr auto BUTTON_INSET_TWICE = BUTTON_INSET * 2;
    constexpr auto CHEVRON_RADIUS = BUTTON_INSET_TWICE * 3.68;
    constexpr auto CONTROLLER_BUTTON_TIP_RADIUS = 16.0;
    constexpr auto CONTROLLER_BUTTON_TIP_INSET = 6.0; // must reduce radius to 10
    constexpr auto CHEVRON_SLICE = 0.375;
    constexpr auto HIGHLIGHT_HALO_SIZE = 1;
    constexpr auto CONTROL_BORDER_WIDTH = 1.31;
    constexpr auto IN_GAME_BUTTON_LABEL_FONT_SIZE = 21;
    constexpr auto CONTROLLER_BUTTON_TIP_FONT_SIZE = 13;
    constexpr auto CONTROLLER_BUTTON_TIP_X = "X";

    extern cairo_pattern_t* selPatternRed;
    extern cairo_pattern_t* selPatternBlue;
    extern cairo_pattern_t* selPatternYellow;

    cairo_t* DrawButton(cairo_t* context, double x, double y, double w, double h, Constants::ControlHighlight highlight);
    cairo_t* DrawControllerButtonTip(cairo_t* context, double x, double y, Constants::ControlHighlight highlight);
    cairo_t* DrawActiveControllerSquare(cairo_t* context, double x, double y, double w, double h);
    void DrawLabel(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_FRect* frame);
    void CenterLabel(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_FRect* frame);
    cairo_t* DrawChevron(cairo_t* context, double centerx, double centery, bool left_or_right, unsigned short nudge);
    void TeardownDialogControls();

    void PrepareSelPatternsInternal();
}
#endif //DIALOG_CONTROLS_H
