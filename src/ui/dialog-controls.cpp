#include "dialog-controls.h"
#include "../../src/shared-constants.h"

namespace blooDot::DialogControls {
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

	cairo_pattern_t* selPatternRed = nullptr; // use in main menu
	cairo_pattern_t* selPatternBlue = nullptr; // used in-game
	cairo_pattern_t* selPatternYellow = nullptr; // used in creator mode

	/// <summary>
	/// Expects the patterns to be created
	/// </summary>
	cairo_t* DrawActiveControllerSquare(cairo_t* context, double x, double y, double w, double h)
	{
		cairo_set_source(context, selPatternBlue);
		cairo_rectangle(context, x, y, w, h);
		cairo_fill(context);

		return context;
	}

	cairo_t* DrawControllerButtonTip(cairo_t* context, double x, double y, Constants::ControlHighlight highlight)
	{
		if (highlight > Constants::CH_NONE)
		{
			PrepareSelPatternsInternal();
			cairo_pattern_t* highlightPattern;
			if (highlight == Constants::CH_INGAME)
			{
				highlightPattern = selPatternBlue;
			}
			else if (highlight == Constants::CH_CREATORMODE)
			{
				highlightPattern = selPatternYellow;
			}
			else
			{
				highlightPattern = selPatternRed;
			}
		}

		COLOR_CONTROL_BACKGROUND;
		cairo_arc(
			context,
			x + CONTROLLER_BUTTON_TIP_INSET,
			y + CONTROLLER_BUTTON_TIP_INSET,
			CONTROLLER_BUTTON_TIP_RADIUS,
			-(M_PI + M_PI_2) + 0.3,
			-0.3
		);

		cairo_fill(context);
		COLOR_CONTROL_BORDER;
		WIDTH_CONTROL_BORDER;
		cairo_arc(
			context,
			x + CONTROLLER_BUTTON_TIP_INSET,
			y + CONTROLLER_BUTTON_TIP_INSET,
			CONTROLLER_BUTTON_TIP_RADIUS - BUTTON_INSET,
			-(M_PI + M_PI_2) + 0.3,
			-0.3
		);

		cairo_stroke(context);

		return context;
	}


	cairo_t* DrawButton(cairo_t* context, const double x, const double y, const double w, const double h, const Constants::ControlHighlight highlight)
	{
		if (highlight > Constants::CH_NONE)
		{
			PrepareSelPatternsInternal();
			cairo_pattern_t* highlightPattern;
			if (highlight == Constants::CH_INGAME)
			{
				highlightPattern = selPatternBlue;
			}
			else if (highlight == Constants::CH_CREATORMODE)
			{
				highlightPattern = selPatternYellow;
			}
			else
			{
				highlightPattern = selPatternRed;
			}

			cairo_rectangle(
				context,
				x - HIGHLIGHT_HALO_SIZE,
				y - HIGHLIGHT_HALO_SIZE,
				w + 2 * HIGHLIGHT_HALO_SIZE,
				h + 2 * HIGHLIGHT_HALO_SIZE
			);

			cairo_set_source(context, highlightPattern);
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

			cairo_set_source(context, highlightPattern);
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

	void DrawLabel(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_FRect* frame)
	{
		frame->x = static_cast<float>(x);
		frame->y = static_cast<float>(y + Constants::LayoutMetricButtonLabelOffsetY);

		SDL_RenderTexture(renderer, texture, nullptr, frame);
	}

	void CenterLabel(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_FRect* const frame)
	{
		auto& rect = (*frame);

		rect.x = static_cast<float>(x) - rect.w / 2;
		rect.y = static_cast<float>(y) - rect.h / 2;

		SDL_RenderTexture(renderer, texture, nullptr, frame);
	}

	cairo_t* DrawChevron(cairo_t* context, const double centerx, const double centery, const bool left_or_right, const unsigned short nudge)
	{
		COLOR_CONTROL_BACKGROUND;
		cairo_set_line_cap(context, CAIRO_LINE_CAP_ROUND);
		cairo_set_line_width(context, BUTTON_INSET_TWICE);
		const auto nudged = ChevronNudge[nudge % (sizeof(ChevronNudge) / sizeof(double))];

		if (left_or_right)
		{
			cairo_arc(
				context,
				centerx + CHEVRON_RADIUS,
				centery,
				CHEVRON_RADIUS,
				M_PI - CHEVRON_SLICE + nudged,
				M_PI + CHEVRON_SLICE + nudged
			);
		}
		else
		{
			cairo_arc_negative(
				context,
				centerx - CHEVRON_RADIUS,
				centery,
				CHEVRON_RADIUS,
				CHEVRON_SLICE + nudged,
				-CHEVRON_SLICE + nudged
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
				M_PI - CHEVRON_SLICE + nudged,
				M_PI+ CHEVRON_SLICE + nudged
			);
		}
		else
		{
			cairo_arc_negative(
				context,
				centerx - CHEVRON_RADIUS,
				centery,
				CHEVRON_RADIUS,
				CHEVRON_SLICE + nudged,
				-CHEVRON_SLICE + nudged
			);
		}

		cairo_stroke(context);

		return context;
	}

	void TeardownDialogControls()
	{
		if (selPatternRed)
		{
			cairo_pattern_destroy(selPatternRed);
		}

		if (selPatternBlue)
		{
			cairo_pattern_destroy(selPatternBlue);
		}

		if (selPatternYellow)
		{
			cairo_pattern_destroy(selPatternYellow);
		}
	}

	void PrepareSelPatternsInternal()
	{
		if (!selPatternRed)
		{
			double j;
			int stride = 1;

			selPatternRed = cairo_pattern_create_linear(0.0, 0.0, 350.0, 350.0);
			selPatternBlue = cairo_pattern_create_linear(0.0, 0.0, 350.0, 350.0);
			selPatternYellow = cairo_pattern_create_linear(0.0, 0.0, 350.0, 350.0);

			for (j = 0.1; j < 1; j += 0.1)
			{
				if ((stride % 2))
				{
					cairo_pattern_add_color_stop_rgb(selPatternRed, j, 0.35, 0.1, 0.1);
					cairo_pattern_add_color_stop_rgb(selPatternBlue, j, 0.1, 0.1, 0.35);
					cairo_pattern_add_color_stop_rgb(selPatternYellow, j, 0.35, 0.35, 0.1);
				}
				else
				{
					cairo_pattern_add_color_stop_rgb(selPatternRed, j, 1, 0.3, 0.2);
					cairo_pattern_add_color_stop_rgb(selPatternBlue, j, 0.2, 0.3, 1);
					cairo_pattern_add_color_stop_rgb(selPatternYellow, j, 1, 1, 0.2);
				}

				++stride;
			}
		}
	}
}