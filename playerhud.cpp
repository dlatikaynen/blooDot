#include "pch.h"
#include "playerhud.h"

extern SDL_Renderer* GameViewRenderer;
extern SettingsStruct Settings;
extern bool isCreatorMode;
extern int blooDot::Player::NumPlayers;

namespace blooDot::Hud
{
	constexpr SDL_Rect Source = { 0,0,49,49 };
	constexpr int const Padding = 9;

	SDL_Texture* LetterboxBackdropL = nullptr;
	SDL_Texture* LetterboxBackdropR = nullptr;
	SDL_Texture* Current[4] = { nullptr, nullptr, nullptr, nullptr };
	SDL_Rect Destination[4];
	SDL_Rect Letterboxes[2];
	int letterboxWidth = 0;
	bool useLetterboxesForHud = false;
	bool doMinimap = false;

	SDL_Texture* Minimap = nullptr;
	SDL_Rect minimapDims = { 0 };
	SDL_Rect minimapSurc = { 0 };
	SDL_Rect minimapDest = { 0 };

	// makermode stuff
	SDL_Texture* DesignLayers = nullptr;
	SDL_Rect designLayersDims = { 0 };
	SDL_Rect designLayersDest = { 0 };
	SDL_Rect designLayerFloorLabelRect{ 0,0,0,0 };
	SDL_Texture* designLayerFloorLabel = nullptr;
	SDL_Texture* designLayerFloorLabelSelected = nullptr;
	SDL_Rect designLayerWallsLabelRect{ 0,0,0,0 };
	SDL_Texture* designLayerWallsLabel = nullptr;
	SDL_Texture* designLayerWallsLabelSelected = nullptr;
	SDL_Rect designLayerRooofLabelRect{ 0,0,0,0 };
	SDL_Texture* designLayerRooofLabel = nullptr;
	SDL_Texture* designLayerRooofLabelSelected = nullptr;
	DingProps selectedDesignLayer = DingProps::Floor;

	/// <summary>
	/// Layout depends on number of players and resolution
	/// Basically, players are always arranged like quadrants in
	/// mathematicians' coordinate systems: anticlockwise starting
	/// with the first on in the upper right quadrant.
	/// </summary>
	bool Initialize()
	{
		doMinimap = Settings::ShowMinimap();
		minimapDest.w = 182;
		minimapDest.h = 182;

		const auto& logicalW = blooDot::Settings::GetLogicalArenaWidth();
		const auto& logicalH = blooDot::Settings::GetLogicalArenaHeight();
		const auto& physiclW = blooDot::Settings::GetPhysicalArenaWidth();
		const auto& physiclH = blooDot::Settings::GetPhysicalArenaHeight();
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

			/* when the letterboxes are sufficiently wide
			 * to accomodate huda/map, we place them in
			 * the otherwise unused bars to the left and right */
			useLetterboxesForHud = letterboxWidth >= 200;
			if (useLetterboxesForHud)
			{
				minimapDest.x = letterboxWidth / 2 - minimapDest.w / 2;
				minimapDest.y = physiclH - minimapDest.h - 2 * Padding - 2 * GRIDUNIT;
			}

			if (!_DrawLetterboxBackdrops(physiclH))
			{
				return false;
			}
		}
		else
		{
			letterboxWidth = 0;

			// top right
			Destination[iP1].x = logicalW - Padding + 3 - Source.w;
			Destination[iP1].y = Padding;

			// top left
			Destination[iP2].x = Padding - 3;
			Destination[iP2].y = Padding;

			// bottom left
			Destination[iP3].x = Destination[iP2].x - 3;
			Destination[iP3].y = logicalH - Padding - Source.h;

			// bottom right
			Destination[iP4].x = Destination[iP1].x + 3;
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

		if(doMinimap) 
		{
			Minimap = blooDot::Res::LoadPicture(GameViewRenderer, CHUNK_KEY_SAMPLE_MAP, &minimapDims);
			if (!Minimap)
			{
				return false;
			}

			minimapSurc.w = minimapDest.w;
			minimapSurc.h = minimapDest.h;
		}

		if (isCreatorMode)
		{
			_InitializeCreatorModeTools();
		}

		return true;
	}

	bool SetDesignLayer(DingProps layer)
	{
		if (selectedDesignLayer != layer)
		{
			selectedDesignLayer = layer;
			_RedrawDesignLayersTool();

			return true;
		}

		return false;
	}

	bool Redraw(int ofPlayerIndex)
	{
		if (SDL_SetRenderTarget(GameViewRenderer, Current[ofPlayerIndex]) < 0)
		{
			const auto targetError = SDL_GetError();
			ReportError("Failed to set a HUD texture as the render target", targetError);
			return false;
		}

		SDL_SetTextureBlendMode(Current[ofPlayerIndex], SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(GameViewRenderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
		SDL_RenderClear(GameViewRenderer);

		auto canvasTexture = BeginRenderDrawing(GameViewRenderer, GRIDUNIT, GRIDUNIT);
		auto const& canvas = GetDrawingSink();
		auto const& midpointX = static_cast<float>(Source.w / 2);
		auto const& midpointY = static_cast<float>(Source.h / 2);

		//auto const& playerInfo = blooDot::Player::GetState(ofPlayerIndex);
		
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
			midpointX + midpointX - 5., midpointY + 7.); // end point

		/* outer arc of SE quadrant */
		cairo_curve_to(
			canvas,
			2.2 * midpointX, 1.5 * midpointY, // first control point
			1.5 * midpointX, 2.2 * midpointY, // second control point
			midpointX + 7., midpointY + midpointY - 5.); // end point

		/* y-axis part of SE quadrant */
		cairo_curve_to(
			canvas,
			midpointX, midpointY + midpointY - 7., // first control point
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
			midpointX + midpointX - 5., midpointY - 7.); // end point

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

		/*
		
		each petal has one inner, and two outer slots which are ~roughly~ the same area
		there are always four petals
		one petal can overfill, killing the contents of the nearest petal and making it its own
		once the player picks up on a seized petal, it reinitializes to its proper category

		*/
		for (auto s = 0; s < 4; ++s)
		{
		}
		
		EndRenderDrawing(GameViewRenderer, canvasTexture, nullptr);
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
			// the placement of the left one is always identical with its own source rect
			SDL_RenderCopy(GameViewRenderer, LetterboxBackdropL, &Letterboxes[0], &Letterboxes[0]);
			SDL_RenderCopy(GameViewRenderer, LetterboxBackdropR, &Letterboxes[0], &Letterboxes[1]);
		}

		if (useLetterboxesForHud)
		{
			if (isCreatorMode)
			{
				/* draw the extra stuff that is only there when
				 * we are operating in creator (maker) mode */
				SDL_RenderCopy(GameViewRenderer, DesignLayers, &designLayersDims, &designLayersDest);
			}
			else
			{
				// HP flowers
				for (auto i = 0; i < blooDot::Player::NumPlayers; ++i)
				{
					SDL_RenderCopy(GameViewRenderer, Current[i], &Source, &Destination[i]);
				}
			}

			// minimap
			if (doMinimap)
			{
				SDL_RenderCopy(GameViewRenderer, Minimap, &minimapSurc, &minimapDest);
			}
		}
	}

	void Teardown()
	{
		for (auto i = 0; i < 4; ++i)
		{
			if (Current[i])
			{
				SDL_DestroyTexture(Current[i]);
			}
		}

		DestroyTexture(&LetterboxBackdropL);
		DestroyTexture(&LetterboxBackdropR);
		DestroyTexture(&Minimap);
		DestroyTexture(&designLayerRooofLabel);
		DestroyTexture(&designLayerRooofLabelSelected);
		DestroyTexture(&designLayerWallsLabel);
		DestroyTexture(&designLayerWallsLabelSelected);
		DestroyTexture(&designLayerFloorLabel);
		DestroyTexture(&designLayerFloorLabelSelected);
		DestroyTexture(&DesignLayers);
	}

	bool _InitializeCreatorModeTools()
	{
		designLayersDims.w = 170;
		designLayersDims.h = 260;
		designLayersDest.x = letterboxWidth / 2 - designLayersDims.w / 2;
		designLayersDest.y = 100;
		designLayersDest.w = designLayersDims.w;
		designLayersDest.h = designLayersDims.h;

		const auto& layersTexture = SDL_CreateTexture(
			GameViewRenderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET,
			designLayersDims.w,
			designLayersDims.h
		);

		if (!layersTexture)
		{
			const auto createError = SDL_GetError();
			ReportError("Failed to create the layers design tool texture", createError);

			return false;
		}

		DesignLayers = layersTexture;
		designLayerFloorLabelSelected = RenderText(
			GameViewRenderer,
			&designLayerFloorLabelRect,
			FONT_KEY_DIALOG_FAT,
			18,
			literalDesignLayerFloorLabel,
			DialogTextColor
		);

		designLayerFloorLabel = RenderText(
			GameViewRenderer,
			&designLayerFloorLabelRect,
			FONT_KEY_DIALOG_FAT,
			18,
			literalDesignLayerFloorLabel,
			DialogTextDisabledColor
		);

		designLayerWallsLabelSelected = RenderText(
			GameViewRenderer,
			&designLayerWallsLabelRect,
			FONT_KEY_DIALOG_FAT,
			18,
			literalDesignLayerWallsLabel,
			DialogTextColor
		);

		designLayerWallsLabel = RenderText(
			GameViewRenderer,
			&designLayerWallsLabelRect,
			FONT_KEY_DIALOG_FAT,
			18,
			literalDesignLayerWallsLabel,
			DialogTextDisabledColor
		);

		designLayerRooofLabelSelected = RenderText(
			GameViewRenderer,
			&designLayerRooofLabelRect,
			FONT_KEY_DIALOG_FAT,
			18,
			literalDesignLayerRooofLabel,
			DialogTextColor
		);

		designLayerRooofLabel = RenderText(
			GameViewRenderer,
			&designLayerRooofLabelRect,
			FONT_KEY_DIALOG_FAT,
			18,
			literalDesignLayerRooofLabel,
			DialogTextDisabledColor
		);

		return _RedrawDesignLayersTool();
	}

	bool _RedrawDesignLayersTool()
	{
		if (SDL_SetRenderTarget(GameViewRenderer, DesignLayers) < 0)
		{
			const auto targetError = SDL_GetError();
			ReportError("Failed to set the layers design tool texture as the render target", targetError);

			return false;
		}

		SDL_SetTextureBlendMode(DesignLayers, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(GameViewRenderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
		SDL_RenderClear(GameViewRenderer);

		auto canvasTexture = BeginRenderDrawing(GameViewRenderer, designLayersDims.w, designLayersDims.h);
		auto const& canvas = GetDrawingSink();

		cairo_set_line_width(canvas, 5.);
		cairo_set_source_rgb(canvas, .4, .4, .1);
		cairo_set_line_join(canvas, CAIRO_LINE_JOIN_ROUND);
		//cairo_rectangle(canvas, designLayersDims.x + 3, designLayersDims.y + 3, designLayersDims.w - 6, designLayersDims.h - 6);
		//cairo_stroke(canvas);

		auto const& yStride = designLayersDims.w / 4.;
		auto const& xExtent = designLayersDims.w / 3.;
		auto const& xSlantExtent = designLayersDims.w / 4.;
		auto const& yExtent = designLayersDims.w / 6;
		double const& baseY = designLayersDims.y + designLayersDims.h;
		auto const& dashWidth = 5.;

		// dashed guides
		cairo_set_line_width(canvas, 2.2);
		cairo_set_source_rgb(canvas, .31, .31, .32);
		cairo_set_dash(canvas, &dashWidth, 1, 0.);
		cairo_move_to(canvas, designLayersDims.x, baseY - yStride * 0);
		cairo_rel_line_to(canvas, 0, -yStride * 2);
		cairo_move_to(canvas, designLayersDims.x + xSlantExtent, baseY - yExtent - yStride * 0);
		cairo_rel_line_to(canvas, 0, -yStride * 2);
		cairo_move_to(canvas, designLayersDims.x + xExtent, baseY - yStride * 0);
		cairo_rel_line_to(canvas, 0, -yStride * 2);
		cairo_move_to(canvas, designLayersDims.x + xExtent + xSlantExtent, baseY - yExtent - yStride * 0);
		cairo_rel_line_to(canvas, 0, -yStride * 2);
		cairo_stroke(canvas);
		cairo_set_dash(canvas, nullptr, 0, 0.);

		auto layer = [canvas, xExtent, dashWidth, yStride, baseY, yExtent, xSlantExtent](int etage, bool selected)
		{
			auto const& widthPadding = 2.2;

			cairo_set_line_width(canvas, widthPadding);
			SetSourceColor(canvas, selected ? DialogTextColor : DialogTextDisabledColor);
			cairo_move_to(canvas, designLayersDims.x + xExtent + dashWidth, baseY - yStride * etage);
			cairo_line_to(canvas, designLayersDims.x + designLayersDims.w, baseY - yStride * etage);
			cairo_stroke(canvas);
			cairo_set_line_width(canvas, 2.6);
			cairo_set_source_rgb(canvas, .3, .2, .6);
			cairo_set_line_join(canvas, CAIRO_LINE_JOIN_BEVEL);
			cairo_move_to(canvas, designLayersDims.x, baseY - yStride * etage);
			cairo_rel_line_to(canvas, xExtent, 0.);
			cairo_rel_line_to(canvas, xSlantExtent, -yExtent);
			cairo_rel_line_to(canvas, -xExtent, 0.);
			cairo_rel_line_to(canvas, -xSlantExtent, yExtent);
			cairo_stroke(canvas);

			if (selected)
			{
				DrawSelectedLayerTool(canvas, designLayersDims.x, baseY, yStride, xExtent, xSlantExtent, yExtent, widthPadding * 2, etage);
			}

			auto const& designLayerLabelPos = SDL_Rect
			{
				designLayersDims.x + designLayersDims.w - designLayerFloorLabelRect.w,
				static_cast<int>(baseY - designLayerFloorLabelRect.h - yStride * etage - widthPadding),
				designLayerFloorLabelRect.w,
				designLayerFloorLabelRect.h
			};

			SDL_Texture* labelTexture = nullptr;
			switch (etage) {
			case 0:
				labelTexture = selected ? designLayerFloorLabelSelected : designLayerFloorLabel;
				break;
					
			case 1:
				labelTexture = selected ? designLayerWallsLabelSelected : designLayerWallsLabel;
				break;

			case 2:
				labelTexture = selected ? designLayerRooofLabelSelected : designLayerRooofLabel;
				break;
			}

			if (labelTexture != nullptr)
			{
				SDL_RenderCopy(GameViewRenderer, labelTexture, NULL, &designLayerLabelPos);
			}
		};

		layer(0, selectedDesignLayer & DingProps::Floor);
		layer(1, selectedDesignLayer & DingProps::Walls);
		layer(2, selectedDesignLayer & DingProps::Rooof);

		EndRenderDrawing(GameViewRenderer, canvasTexture, nullptr);

		if (SDL_SetRenderTarget(GameViewRenderer, NULL) < 0)
		{
			const auto resetError = SDL_GetError();
			ReportError("Failed to reset the render target after rendering to layers design tool texture", resetError);

			return false;
		}

		return true;
	}

	bool _DrawLetterboxBackdrops(int height)
	{
		LetterboxBackdropL = SDL_CreateTexture(
			GameViewRenderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET,
			letterboxWidth,
			height
		);

		if (!LetterboxBackdropL)
		{
			const auto backdropError = SDL_GetError();
			ReportError("Failed to create near HUD backdrop texture", backdropError);
			return false;
		}

		LetterboxBackdropR = SDL_CreateTexture(
			GameViewRenderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET,
			letterboxWidth,
			height
		);

		if (!LetterboxBackdropR)
		{
			const auto backdropError = SDL_GetError();
			ReportError("Failed to create far HUD backdrop texture", backdropError);
			return false;
		}

		/* draw the mostly static parts of the left letterbox */
		if (SDL_SetRenderTarget(GameViewRenderer, LetterboxBackdropL) < 0)
		{
			const auto targetError = SDL_GetError();
			ReportError("Failed to set near HUD letterbox backdrop texture as the render target", targetError);
			return false;
		}

		SDL_SetRenderDrawColor(GameViewRenderer, 0x48 / 2, 0x3d / 2, 0x8b / 2, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(GameViewRenderer);
		SDL_SetRenderDrawColor(GameViewRenderer, 0x48 / 3, 0x3d / 3, 0x8b / 3, SDL_ALPHA_OPAQUE);
		SDL_Rect border = { letterboxWidth - 3, 0, 3, height };
		SDL_RenderFillRect(GameViewRenderer, &border);
		SDL_SetRenderDrawColor(GameViewRenderer, static_cast<Uint8>(0x48 / 2.5), static_cast<Uint8>(0x3d / 2.5), static_cast<Uint8>(0x8b / 2.5), SDL_ALPHA_OPAQUE);
		SDL_Rect fineBorder = { letterboxWidth - 5, 0, 1, height };
		SDL_RenderFillRect(GameViewRenderer, &fineBorder);
		
		if (doMinimap)
		{
			SDL_Rect minimapHole = { minimapDest.x - 6,minimapDest.y - 6,minimapDest.w + 12,minimapDest.h + 12 };
			SDL_Rect minimapChrome = { minimapDest.x - 10,minimapDest.y - 10,minimapDest.w + 20,minimapDest.h + 20 };
			SDL_SetRenderDrawBlendMode(GameViewRenderer, SDL_BLENDMODE_MOD);
			SDL_SetRenderDrawColor(GameViewRenderer, 0x0e, 0x0d, 0x0d, SDL_ALPHA_OPAQUE);
			SDL_RenderFillRect(GameViewRenderer, &minimapHole);
			SDL_SetRenderDrawBlendMode(GameViewRenderer, SDL_BLENDMODE_BLEND);

			const auto& chrome = BeginRenderDrawing(GameViewRenderer, minimapChrome.w, minimapChrome.h);
			const auto& canvas = GetDrawingSink();
			cairo_move_to(canvas, 5, minimapChrome.h - 5);
			cairo_curve_to(canvas,
				1, minimapChrome.h / static_cast<double>(2),
				1, minimapChrome.h / static_cast<double>(2),
				5, 5
			);

			cairo_curve_to(canvas,
				minimapChrome.w / 2, 1,
				minimapChrome.w / 2, 1,
				minimapChrome.w - 5, 5
			);

			cairo_curve_to(canvas,
				minimapChrome.w - 1, minimapChrome.h / static_cast<double>(2),
				minimapChrome.w - 1, minimapChrome.h / static_cast<double>(2),
				minimapChrome.w - 5, minimapChrome.h - 5
			);

			cairo_curve_to(canvas,
				minimapChrome.w / static_cast<double>(2), minimapChrome.h - 1,
				minimapChrome.w / static_cast<double>(2), minimapChrome.h - 1,
				5, minimapChrome.h - 5
			);

			cairo_set_line_width(canvas, 3);
			cairo_set_source_rgb(
				canvas,
			    (static_cast<double>(0x48) / 3) / 255.,
				(static_cast<double>(0x3d) / 3) / 255.,
				(static_cast<double>(0x8b) / 3) / 255.
			);

			cairo_stroke(canvas);

			EndRenderDrawing(GameViewRenderer, chrome, &minimapChrome);
		}

		/* draw the mostly static parts of the right letterbox */
		if (SDL_SetRenderTarget(GameViewRenderer, LetterboxBackdropR) < 0)
		{
			const auto targetError = SDL_GetError();
			ReportError("Failed to set far HUD letterbox backdrop texture as the render target", targetError);
			return false;
		}

		SDL_SetRenderDrawColor(GameViewRenderer, 0x48 / 2, 0x3d / 2, 0x8b / 2, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(GameViewRenderer);
		SDL_SetRenderDrawColor(GameViewRenderer, 0x48 / 3, 0x3d / 3, 0x8b / 3, SDL_ALPHA_OPAQUE);
		border = { 0, 0, 3, height };
		SDL_RenderFillRect(GameViewRenderer, &border);
		SDL_SetRenderDrawColor(GameViewRenderer, static_cast<Uint8>(0x48 / 2.5), static_cast<Uint8>(0x3d / 2.5), static_cast<Uint8>(0x8b / 2.5), SDL_ALPHA_OPAQUE);
		fineBorder = { 4, 0, 1, height };
		SDL_RenderFillRect(GameViewRenderer, &fineBorder);

		/* cleanup */
		if (SDL_SetRenderTarget(GameViewRenderer, NULL) < 0)
		{
			const auto resetError = SDL_GetError();
			ReportError("Failed to reset the render target after rendering to HUD letterbox backdrop", resetError);
			return false;
		}

		return true;
	}
}