#include "pch.h"
#include "dialogcontrols.h"
#include <math.h>

constexpr auto HALFPI = M_PI / 2.0;
constexpr auto BUTTON_INSET = 4.0;
constexpr auto BUTTON_INSET_TWICE = BUTTON_INSET * 2;
constexpr auto CHEVRON_RADIUS = BUTTON_INSET_TWICE * 3.68;
constexpr auto CHEVRON_SLICE = 0.375;
constexpr auto HIGHLIGHT_HALO_SIZE = 1;
constexpr auto CONTROL_BORDER_WIDTH = 1.31;

constexpr double ChevronNudge[] = {
	0,
	0.05,0.06,0.065,0.07,0.07,0.065,0.06,0.05,
	0,0,0,
	-0.05,-0.06,-0.065,-0.07,-0.07,-0.065,-0.06,-0.05,
	0,0 
};

#define COLOR_CONTROL_BACKGROUND cairo_set_source_rgba(context, 0.1, 0.1, 0.11, 0.98)
#define COLOR_CONTROL_HIGHLIGHT cairo_set_source_rgba(context, 0.9, 0.9, 0.91, 0.76)
#define COLOR_CONTROL_BORDER cairo_set_source_rgb(context, 1.0, 1.0, 1.0)
#define WIDTH_CONTROL_BORDER cairo_set_line_width(context, CONTROL_BORDER_WIDTH)

cairo_pattern_t* selPattern = NULL;

cairo_t* DrawButton(cairo_t* context, double x, double y, double w, double h, bool highlighted)
{
	if (highlighted)
	{
		_PrepareSelPattern();
		cairo_rectangle(
			context,
			x - HIGHLIGHT_HALO_SIZE,
			y - HIGHLIGHT_HALO_SIZE,
			w + 2 * HIGHLIGHT_HALO_SIZE,
			h + 2 * HIGHLIGHT_HALO_SIZE
		);

		cairo_set_source(context, selPattern);
		cairo_fill(context);

		COLOR_CONTROL_BACKGROUND;
		cairo_rectangle(context, x, y, w, h);
		cairo_fill(context);

		cairo_rectangle(
			context,
			x + BUTTON_INSET,
			y + BUTTON_INSET,
			w - BUTTON_INSET_TWICE,
			h - BUTTON_INSET_TWICE
		);

		cairo_set_source(context, selPattern);
		cairo_fill(context);

		COLOR_CONTROL_BACKGROUND;
		cairo_rectangle(
			context,
			x + BUTTON_INSET + CONTROL_BORDER_WIDTH,
			y + BUTTON_INSET + CONTROL_BORDER_WIDTH,
			w - BUTTON_INSET_TWICE - 2 * CONTROL_BORDER_WIDTH,
			h - BUTTON_INSET_TWICE - 2 * CONTROL_BORDER_WIDTH
		);

		cairo_fill(context);
		
		COLOR_CONTROL_HIGHLIGHT;
		WIDTH_CONTROL_BORDER;
		cairo_rectangle(
			context,
			x + BUTTON_INSET,
			y + BUTTON_INSET,
			w - BUTTON_INSET_TWICE,
			h - BUTTON_INSET_TWICE
		);

		cairo_stroke(context);
	}
	else
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
	}

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

void TeardownDialogControls()
{
	if (selPattern)
	{
		cairo_pattern_destroy(selPattern);
	}
}

void _PrepareSelPattern()
{
	if (!selPattern)
	{
		double j;
		int stride = 1;

		selPattern = cairo_pattern_create_linear(0.0, 0.0, 350.0, 350.0);

		for (j = 0.1; j < 1; j += 0.1)
		{
			if ((stride % 2))
			{
				cairo_pattern_add_color_stop_rgb(selPattern, j, 0.35, 0.1, 0.1);
			}
			else
			{
				cairo_pattern_add_color_stop_rgb(selPattern, j, 1, 0.3, 0.2);
			}

			++stride;
		}
	}
}