#include "pch.h"
#include "dialogcontrols.h"
#include "constants.h"
#include <math.h>

namespace blooDot::MakerControls
{
	cairo_t* DrawSelectedLayerTool(cairo_t* context, int x, double baseY, double yStride, double xExtent, double xSlantExtent, int yExtent, double padding, int etage)
	{
		auto const& intendedX = x + 2 * padding;
		auto const& intendedY = baseY - padding - yStride * etage;

		/* we shift over the pattern so it appears darker the further down we get */
		auto const& patternX = 45 * (2 - etage);

		_PrepareSelPatterns();
		cairo_translate(context, intendedX - patternX, intendedY - 100);
		cairo_set_source(context, selPatternYellow); // this is always only in the design mode
		cairo_pattern_set_extend(cairo_get_source(context), CAIRO_EXTEND_REFLECT);
		cairo_move_to(context, patternX, 100);
		cairo_rel_line_to(context, xExtent - 2 * padding, 0.);
		cairo_rel_line_to(context, xSlantExtent - 2 * padding, -yExtent + 2 * padding);
		cairo_rel_line_to(context, -xExtent + 2 * padding, 0.);
		cairo_line_to(context, patternX, 100);
		cairo_translate(context, -intendedX, -intendedY);
		cairo_fill(context);
		cairo_translate(context, patternX, 100);

		return context;
	}
}