#pragma once
#include "enums.h"
#include <SDL2/SDL.h>
#include <cairo/cairo.h>

constexpr auto const HALFPI = M_PI / 2.0;
constexpr auto const BUTTON_INSET = 4.0;
constexpr auto const BUTTON_INSET_TWICE = BUTTON_INSET * 2;
constexpr auto const CHEVRON_RADIUS = BUTTON_INSET_TWICE * 3.68;
constexpr auto const CONTROLLER_BUTTON_TIP_RADIUS = 16.0;
constexpr auto const CONTROLLER_BUTTON_TIP_INSET = 6.0; // must reduce radius to 10
constexpr auto const CHEVRON_SLICE = 0.375;
constexpr auto const HIGHLIGHT_HALO_SIZE = 1;
constexpr auto const CONTROL_BORDER_WIDTH = 1.31;
constexpr auto const IN_GAME_BUTTON_LABEL_FONT_SIZE = 21;
constexpr auto const CONTROLLER_BUTTON_TIP_FONT_SIZE = 13;
constexpr const char* CONTROLLER_BUTTON_TIP_X = "X";

extern cairo_pattern_t* selPatternRed;
extern cairo_pattern_t* selPatternBlue;
extern cairo_pattern_t* selPatternYellow;

cairo_t* DrawButton(cairo_t* context, double x, double y, double w, double h, ControlHighlight highlight);
cairo_t* DrawControllerButtonTip(cairo_t* context, double x, double y, ControlHighlight highlight);
cairo_t* DrawActiveControllerSquare(cairo_t* context, double x, double y, double w, double h);
void DrawLabel(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_Rect* frame);
void CenterLabel(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_Rect* frame);
cairo_t* DrawChevron(cairo_t* context, double centerx, double centery, bool left_or_right, unsigned short nudge);
void TeardownDialogControls();

void _PrepareSelPatterns();