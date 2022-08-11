#include "pch.h"
#include "dialogcontrols.h"
#include <math.h>

cairo_t* DrawButtonChevron(cairo_t* context)
{
	const auto halfPi = M_PI / 2;
	cairo_set_antialias(context, CAIRO_ANTIALIAS_SUBPIXEL);
	cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
	cairo_set_line_width(context, 2.35f);
	
	cairo_arc(context, 80, 60, 20, halfPi, 3 * halfPi);

	cairo_stroke(context);

	return context;
}
