#include "pch.h"
#include "dialogcontrols.h"
#include <math.h>

constexpr auto BUTTON_INSET = 4.0;
constexpr auto BUTTON_INSET_TWICE = BUTTON_INSET * 2;

cairo_t* DrawButton(cairo_t* context, double x, double y, double w, double h)
{
	const auto halfPi = M_PI / 2;
	cairo_set_antialias(context, CAIRO_ANTIALIAS_SUBPIXEL);
	cairo_set_source_rgba(context, 0.1, 0.1, 0.11, 0.98);
	cairo_rectangle(context, x, y, w, h);
	cairo_fill(context);
	cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
	cairo_set_line_width(context, 1.31);
	cairo_rectangle(
		context,
		x + BUTTON_INSET,
		y + BUTTON_INSET,
		w - BUTTON_INSET_TWICE,
		h - BUTTON_INSET_TWICE
	);

	cairo_stroke(context);


	
	cairo_arc(context, 80, 60, 20, halfPi, 3 * halfPi);
	cairo_stroke(context);


	return context;
}

void DrawLabel(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, SDL_Rect* frame)
{
	(*frame).x = x;
	(*frame).y = y;

	SDL_RenderCopy(renderer, texture, NULL, frame);
}