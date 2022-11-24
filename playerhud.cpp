#include "pch.h"
#include "playerhud.h"
#include "playerstate.h"
#include "settings.h"
#include "drawing.h"

extern SDL_Renderer* GameViewRenderer;
extern SettingsStruct Settings;
extern int blooDot::Player::NumPlayers;

namespace blooDot::Hud
{
	constexpr SDL_Rect Source = { 0,0,49,49 };
	constexpr int const Padding = 9;
	constexpr Uint8 const LetterboxGray = 0xa;

	SDL_Texture* Current[4];
	SDL_Rect Destination[4];
	SDL_Rect Letterboxes[2];
	int letterboxWidth = 0;

	/// <summary>
	/// Layout depends on number of players and resolution
	/// Basically, players are always arranged like quadrants in
	/// mathematicians' coordinate systems: anticlockwise starting
	/// with the first on in the upper right quadrant.
	/// </summary>
	bool Initialize()
	{
		const auto& logicalW = blooDot::Settings::GetLogicalArenaWidth();
		const auto& logicalH = blooDot::Settings::GetLogicalArenaHeight();
		const auto& physiclW = blooDot::Settings::GetPhysicalArenaWidth();
		const auto& availabW = physiclW - logicalW;
		if (availabW >= (Source.w * 3 / 2))
		{
			letterboxWidth = availabW / 2;
			Letterboxes[0].x = 0;
			Letterboxes[0].y = 0;
			Letterboxes[0].w = letterboxWidth;
			Letterboxes[0].h = logicalH;
			Letterboxes[1].x = physiclW - letterboxWidth;
			Letterboxes[1].y = 0;
			Letterboxes[1].w = letterboxWidth;
			Letterboxes[1].h = logicalH;

			// top right
			Destination[iP1].x = physiclW - letterboxWidth + Padding;
			Destination[iP1].y = Padding;

			// top left
			Destination[iP2].x = letterboxWidth - Padding - Source.w;
			Destination[iP2].y = Padding;

			// bottom left
			Destination[iP3].x = Destination[iP2].x;
			Destination[iP3].y = logicalH - Padding - Source.h;

			// bottom right
			Destination[iP4].x = Destination[iP1].x;
			Destination[iP4].y = Destination[iP3].y;
		}
		else
		{
			letterboxWidth = 0;

			// top right
			Destination[iP1].x = logicalW - Padding - Source.w;
			Destination[iP1].y = Padding;

			// top left
			Destination[iP2].x = Padding;
			Destination[iP2].y = Padding;

			// bottom left
			Destination[iP3].x = Destination[iP2].x;
			Destination[iP3].y = logicalH - Padding - Source.h;

			// bottom right
			Destination[iP4].x = Destination[iP1].x;
			Destination[iP4].y = Destination[iP3].y;
		}

		for (auto i = 0; i < blooDot::Player::NumPlayers; ++i)
		{
			Destination[i].w = Source.w;
			Destination[i].h = Source.h;

			const auto& hudTexture = SDL_CreateTexture(
				GameViewRenderer,
				SDL_PIXELFORMAT_ARGB8888,
				SDL_TEXTUREACCESS_TARGET,
				Source.w,
				Source.h
			);

			if (!hudTexture)
			{
				const auto createError = SDL_GetError();
				ReportError("Failed to create a HUD texture", createError);
				return false;
			}

			Current[i] = hudTexture;
			if (!Redraw(i))
			{
				return false;
			}
		}

		return true;
	}

	bool Redraw(int ofPlayerIndex)
	{
		if (SDL_SetRenderTarget(GameViewRenderer, Current[ofPlayerIndex]) < 0)
		{
			const auto targetError = SDL_GetError();
			ReportError("Failed to set a HUD texture as the render target", targetError);
			return false;
		}

		SDL_SetRenderDrawColor(GameViewRenderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
		SDL_RenderClear(GameViewRenderer);

		auto canvasTexture = BeginRenderDrawing(GameViewRenderer, GRIDUNIT, GRIDUNIT);
		auto const& canvas = GetDrawingSink();
		auto const& midpointX = static_cast<float>(Source.w / 2);
		auto const& midpointY = static_cast<float>(Source.h / 2);

		auto const& playerInfo = blooDot::Player::GetState(ofPlayerIndex);
		
		cairo_set_line_width(canvas, 3.);
		cairo_set_source_rgb(canvas, .2, .7, .2);
		//cairo_arc(canvas, midpointX, midpointY, midpointY - 5, 0, M_PI * 2);
		//cairo_stroke(canvas);

		cairo_set_line_width(canvas, 1.3);
		cairo_set_source_rgb(canvas, 0.7, 1., 0);

		/* x-axis part of SE quadrant */
		cairo_move_to(canvas, midpointX, midpointY);
		cairo_curve_to(
			canvas,
			midpointX, midpointY, // first control point
			midpointX + midpointX - 7., midpointY, // second control point
			midpointX + midpointX - 5., midpointY + 7); // end point

		/* outer arc of SE quadrant */
		cairo_curve_to(
			canvas,
			2.2 * midpointX, 1.5 * midpointY, // first control point
			1.5 * midpointX, 2.2 * midpointY, // second control point
			midpointX + 7, midpointY + midpointY - 5.); // end point

		/* y-axis part of SE quadrant */
		cairo_curve_to(
			canvas,
			midpointX, midpointY + midpointY - 7, // first control point
			midpointX, midpointY, // second control point
			midpointX, midpointY); // end point

		cairo_stroke(canvas);
		cairo_set_source_rgb(canvas, .97, .03, .03);

		/* x-axis part of NE quadrant */
		cairo_move_to(canvas, midpointX, midpointY);
		cairo_curve_to(
			canvas,
			midpointX, midpointY, // first control point
			midpointX + midpointX - 7., midpointY, // second control point
			midpointX + midpointX - 5., midpointY - 7); // end point

		/* outer arc of NE quadrant */
		cairo_curve_to(
			canvas,
			2.2 * midpointX, midpointY - 0.5 * midpointY, // first control point
			1.5 * midpointX, midpointY - 1.2 * midpointY, // second control point
			midpointX + 7., 5.); // end point

		/* y-axis part of NE quadrant */
		cairo_curve_to(
			canvas,
			midpointX, 7., // first control point
			midpointX, midpointY, // second control point
			midpointX, midpointY); // end point

		cairo_stroke(canvas);
		cairo_set_source_rgb(canvas, 1., .65, .2);

		/* x-axis part of SW quadrant */
		cairo_move_to(canvas, midpointX, midpointY);
		cairo_curve_to(
			canvas,
			midpointX, midpointY, // first control point
			7., midpointY, // second control point
			5., midpointY + 7.); // end point

		/* outer arc of SW quadrant */
		cairo_curve_to(
			canvas,
			midpointX - 1.2 * midpointX, 1.5 * midpointY, // first control point
			midpointX - 0.5 * midpointX, 2.2 * midpointY, // second control point
			midpointX - 7, midpointY + midpointY - 5.); // end point

		/* y-axis part of SW quadrant */
		cairo_curve_to(
			canvas,
			midpointX, midpointY + midpointY - 7, // first control point
			midpointX, midpointY, // second control point
			midpointX, midpointY); // end point

		cairo_stroke(canvas);
		cairo_set_source_rgb(canvas, .43, .68, .94);

		/* x-axis part of NW quadrant */
		cairo_move_to(canvas, midpointX, midpointY);
		cairo_curve_to(
			canvas,
			midpointX, midpointY, // first control point
			7., midpointY, // second control point
			5., midpointY - 7.); // end point

		/* outer arc of NW quadrant */
		cairo_curve_to(
			canvas,
			midpointX - 1.2 * midpointX, midpointY - 0.5 * midpointY, // first control point
			midpointX - 0.5 * midpointX, midpointY - 1.2 * midpointY, // second control point
			midpointX - 7, 5.); // end point

		/* y-axis part of NW quadrant */
		cairo_curve_to(
			canvas,
			midpointX, 7., // first control point
			midpointX, midpointY, // second control point
			midpointX, midpointY); // end point

		cairo_stroke(canvas);

		for (auto s = 0; s < playerInfo->Slots; ++s)
		{
		}
		
		EndRenderDrawing(GameViewRenderer, canvasTexture);
		if (SDL_SetRenderTarget(GameViewRenderer, NULL) < 0)
		{
			const auto resetError = SDL_GetError();
			ReportError("Failed to reset the render target after rendering to HUD texture", resetError);
			return false;
		}

		return true;
	}

	void Render()
	{
		if (letterboxWidth > 0)
		{
			SDL_SetRenderDrawColor(
				GameViewRenderer,
				LetterboxGray,
				LetterboxGray,
				LetterboxGray,
				SDL_ALPHA_OPAQUE
			);

			SDL_RenderFillRects(GameViewRenderer, Letterboxes, 2);
		}

		for (auto i = 0; i < blooDot::Player::NumPlayers; ++i)
		{
			SDL_RenderCopy(GameViewRenderer, Current[i], &Source, &Destination[i]);
		}
	}
}