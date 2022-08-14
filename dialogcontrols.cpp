#include "pch.h"
#include "dialogcontrols.h"
#include <math.h>

constexpr auto HALFPI = M_PI / 2.0;
constexpr auto BUTTON_INSET = 4.0;
constexpr auto BUTTON_INSET_TWICE = BUTTON_INSET * 2;
constexpr auto CHEVRON_RADIUS = BUTTON_INSET_TWICE * 3.68;
constexpr auto CHEVRON_SLICE = 0.375;

constexpr double ChevronNudge[] = { 0,0.05,0.06,0.065,0.07,0.07,0.065,0.06,0.05,0,0,0,-0.05,-0.06,-0.065,-0.07,-0.07,-0.065,-0.06,-0.05,0,0 };

#define COLOR_CONTROL_BACKGROUND cairo_set_source_rgba(context, 0.1, 0.1, 0.11, 0.98)
#define COLOR_CONTROL_BORDER cairo_set_source_rgb(context, 1.0, 1.0, 1.0)
#define WIDTH_CONTROL_BORDER cairo_set_line_width(context, 1.31)

cairo_t* DrawButton(cairo_t* context, double x, double y, double w, double h)
{
	COLOR_CONTROL_BACKGROUND;
	cairo_rectangle(context, x, y, w, h);
	cairo_fill(context);
	COLOR_CONTROL_BORDER;
	WIDTH_CONTROL_BORDER;
	cairo_rectangle(
		context,
		x + BUTTON_INSET,
		y + BUTTON_INSET,
		w - BUTTON_INSET_TWICE,
		h - BUTTON_INSET_TWICE
	);

	cairo_stroke(context);

	return context;
}

void DrawLabel(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_Rect* frame)
{
	(*frame).x = x;
	(*frame).y = y;

	SDL_RenderCopy(renderer, texture, NULL, frame);
}

cairo_t* DrawChevron(cairo_t* context, double centerx, double centery, bool left_or_right, unsigned short nudgeFrame)
{
	COLOR_CONTROL_BACKGROUND;
	cairo_set_line_cap(context, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_width(context, BUTTON_INSET_TWICE);
	const auto nudge = ChevronNudge[nudgeFrame % (sizeof(ChevronNudge) / sizeof(double))];

	if (left_or_right)
	{
		cairo_arc(
			context,
			centerx + CHEVRON_RADIUS,
			centery,
			CHEVRON_RADIUS,
			M_PI - CHEVRON_SLICE + nudge,
			M_PI + CHEVRON_SLICE + nudge
		);
	}
	else
	{
		cairo_arc_negative(
			context,
			centerx - CHEVRON_RADIUS,
			centery,
			CHEVRON_RADIUS,
			CHEVRON_SLICE + nudge,
			-CHEVRON_SLICE + nudge
		);
	}

	cairo_stroke(context);

	COLOR_CONTROL_BORDER;
	WIDTH_CONTROL_BORDER;
	if (left_or_right)
	{
		cairo_arc(
			context,
			centerx + CHEVRON_RADIUS,
			centery, CHEVRON_RADIUS,
			M_PI - CHEVRON_SLICE + nudge,
			M_PI+ CHEVRON_SLICE + nudge
		);
	}
	else
	{
		cairo_arc_negative(
			context,
			centerx - CHEVRON_RADIUS,
			centery,
			CHEVRON_RADIUS,
			CHEVRON_SLICE + nudge,
			-CHEVRON_SLICE + nudge
		);
	}

	cairo_stroke(context);

	return context;
}
