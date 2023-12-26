#pragma once
#include "enums.h"
#include <SDL2/SDL.h>
#include <cairo/cairo.h>

namespace blooDot::MakerControls
{
	cairo_t* DrawSelectedLayerTool(cairo_t* context, int x, double baseY, double yStride, double xExtent, double xSlantExtent, int yExtent, double padding, int etage);
}