#include "pch.h"
#include "playerhud.h"
#include "makercontrols.h"
#include "dingsheet.h"

using namespace blooDot::ClumsyPack;

extern SDL_Renderer* GameViewRenderer;
extern SettingsStruct Settings;
extern bool isCreatorMode;
extern int blooDot::Player::NumPlayers;

namespace blooDot::Hud
{
	constexpr const auto LESSER_GU = GRIDUNIT / 3 * 2;
	constexpr SDL_Rect Source = { 0,0,GRIDUNIT,GRIDUNIT };

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
	SDL_Texture* DingSelector = nullptr;
	SDL_Rect dingSelectorDims = { 0 };
	SDL_Rect dingSelectorDest = { 0 };
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
	SDL_Rect gridlockLabelRect{ 0,0,0,0 };
	SDL_Texture* gridlockLabel = nullptr;
	SDL_Texture* gridlockLabelOn = nullptr;
	SDL_Rect coalesceLabelRect{ 0,0,0,0 };
	SDL_Texture* coalesceLabel = nullptr;
	SDL_Texture* coalesceLabelOn = nullptr;
	SDL_Rect designerDingBox{ 0,0,0,0 };
	Ding designerCurrentDing = Ding::FloorRockCracked;

	// makermode ui state
	DingProps selectedDesignLayer = DingProps::Floor;
	bool isGridlockOn = true; // whether or not placements are locked to the arena grid
	bool isCoalesceOn = true; // whether or not touching coalesceable items coalesce

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
			if (!_InitializeCreatorModeTools())
			{
				return false;
			}

			if (!_InitializeDingSelectorTool())
			{
				return false;
			}
		}

		return true;
	}

	DingProps GetDesignLayer()
	{
		return selectedDesignLayer;
	}

	bool DesignLayerUp()
	{
		switch (selectedDesignLayer)
		{
		case DingProps::Floor:
			return SetDesignLayer(DingProps::Floor);

		case DingProps::Walls:
			return SetDesignLayer(DingProps::Rooof);
		}

		return false;
	}

	bool DesignLayerDown()
	{
		switch (selectedDesignLayer)
		{
		case DingProps::Walls:
			return SetDesignLayer(DingProps::Floor);

		case DingProps::Rooof:
			return SetDesignLayer(DingProps::Walls);
		}

		return false;
	}

	bool SetDesignLayer(DingProps layer)
	{
		if (selectedDesignLayer != layer)
		{
			selectedDesignLayer = layer;
			if (!_RedrawDesignLayersTool())
			{
				return false;
			}

			if (!_RedrawDingSelectorTool())
			{
				return false;
			}

			return true;
		}

		return false;
	}

	bool GetGridLock()
	{
		return isGridlockOn;
	}

	bool ToggleGridLock()
	{
		return SetGridlock(!isGridlockOn);
	}

	bool SetGridlock(bool gridLock)
	{
		if (isGridlockOn != gridLock)
		{
			isGridlockOn = gridLock;
			_RedrawDesignLayersTool();

			return true;
		}

		return false;
	}

	bool GetCoalesce()
	{
		return isCoalesceOn;
	}

	bool ToggleCoalesce()
	{
		return SetCoalesce(!isCoalesceOn);
	}

	bool SetCoalesce(bool coalesce)
	{
		if (isCoalesceOn != coalesce)
		{
			isCoalesceOn = coalesce;
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
				SDL_RenderCopy(GameViewRenderer, DingSelector, &dingSelectorDims, &dingSelectorDest);
				SDL_RenderCopy(GameViewRenderer, DesignLayers, &designLayersDims, &designLayersDest);

				const auto dingLocator = GetDing(designerCurrentDing);
				if (dingLocator->onSheet)
				{
					SDL_Rect dst = {
						dingSelectorDest.x + designerDingBox.x + designerDingBox.w / 2 - dingLocator->src.w / 2,
						dingSelectorDest.y + designerDingBox.y + designerDingBox.h / 2 - dingLocator->src.h / 2,
						dingLocator->src.w,
						dingLocator->src.h
					};

					SDL_RenderCopy(
						GameViewRenderer,
						dingLocator->onSheet,
						&dingLocator->src,
						&dst
					);
				}
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

		if (isCreatorMode)
		{
			DestroyTexture(&gridlockLabel);
			DestroyTexture(&gridlockLabelOn);
			DestroyTexture(&coalesceLabel);
			DestroyTexture(&coalesceLabelOn);
			DestroyTexture(&designLayerRooofLabel);
			DestroyTexture(&designLayerRooofLabelSelected);
			DestroyTexture(&designLayerWallsLabel);
			DestroyTexture(&designLayerWallsLabelSelected);
			DestroyTexture(&designLayerFloorLabel);
			DestroyTexture(&designLayerFloorLabelSelected);
			DestroyTexture(&DesignLayers);
			DestroyTexture(&DingSelector);
		}
	}

	bool _InitializeDingSelectorTool()
	{
		dingSelectorDims.w = 2 + 7 * LESSER_GU + 6 * Padding + 2;
		dingSelectorDims.h = dingSelectorDims.w;
		dingSelectorDest.x = letterboxWidth / 2 - dingSelectorDims.w / 2;
		dingSelectorDest.y = 49;
		dingSelectorDest.w = dingSelectorDims.w;
		dingSelectorDest.h = dingSelectorDims.h;

		const auto& selectorTexture = SDL_CreateTexture(
			GameViewRenderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET,
			dingSelectorDims.w,
			dingSelectorDims.h
		);

		if (!selectorTexture)
		{
			const auto createError = SDL_GetError();
			ReportError("Failed to create the ding selector tool texture", createError);

			return false;
		}

		DingSelector = selectorTexture;

		return _RedrawDingSelectorTool();
	}

	bool _InitializeCreatorModeTools()
	{
		designLayersDims.w = minimapDest.w;
		designLayersDims.h = 280;
		designLayersDest.x = letterboxWidth / 2 - designLayersDims.w / 2;
		designLayersDest.y = Letterboxes[0].h / 2 - designLayersDims.h / 2;
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

		gridlockLabelOn = RenderText(
			GameViewRenderer,
			&gridlockLabelRect,
			FONT_KEY_DIALOG_FAT,
			18,
			literalDesignGridlockLabel,
			DialogTextColor
		);

		gridlockLabel = RenderText(
			GameViewRenderer,
			&gridlockLabelRect,
			FONT_KEY_DIALOG_FAT,
			18,
			literalDesignGridlockLabel,
			DialogTextDisabledColor
		);

		coalesceLabelOn = RenderText(
			GameViewRenderer,
			&coalesceLabelRect,
			FONT_KEY_DIALOG_FAT,
			18,
			literalDesignCoalesceLabel,
			DialogTextColor
		);

		coalesceLabel = RenderText(
			GameViewRenderer,
			&coalesceLabelRect,
			FONT_KEY_DIALOG_FAT,
			18,
			literalDesignCoalesceLabel,
			DialogTextDisabledColor
		);

		return _RedrawDesignLayersTool();
	}

	bool _RedrawDingSelectorTool()
	{
		if (SDL_SetRenderTarget(GameViewRenderer, DingSelector) < 0)
		{
			const auto targetError = SDL_GetError();
			ReportError("Failed to set the ding selector tool texture as the render target", targetError);

			return false;
		}

		SDL_SetTextureBlendMode(DingSelector, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(GameViewRenderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
		SDL_RenderClear(GameViewRenderer);

		auto canvasTexture = BeginRenderDrawing(GameViewRenderer, dingSelectorDims.w, dingSelectorDims.h);
		auto const& canvas = GetDrawingSink();

		cairo_set_line_width(canvas, 2.6);
		cairo_set_source_rgb(canvas, .3, .2, .6);
		cairo_set_line_join(canvas, CAIRO_LINE_JOIN_BEVEL);

		SDL_Rect biome1 = { dingSelectorDims.x + 2, dingSelectorDims.y + 2 + 0 * (LESSER_GU + Padding), LESSER_GU, LESSER_GU };
		SDL_Rect biome2 = { dingSelectorDims.x + 2, dingSelectorDims.y + 2 + 1 * (LESSER_GU + Padding), LESSER_GU, LESSER_GU };
		SDL_Rect biome3 = { dingSelectorDims.x + 2, dingSelectorDims.y + 2 + 2 * (LESSER_GU + Padding), LESSER_GU, LESSER_GU };
		SDL_Rect biome4 = { dingSelectorDims.x + 2, dingSelectorDims.y + 2 + 3 * (LESSER_GU + Padding), LESSER_GU, LESSER_GU };
		SDL_Rect biome5 = { dingSelectorDims.x + 2, dingSelectorDims.y + 2 + 4 * (LESSER_GU + Padding), LESSER_GU, LESSER_GU };
		SDL_Rect biome6 = { dingSelectorDims.x + 2, dingSelectorDims.y + 2 + 5 * (LESSER_GU + Padding), LESSER_GU, LESSER_GU };
		SDL_Rect biome7 = { dingSelectorDims.x + 2, dingSelectorDims.y + 2 + 6 * (LESSER_GU + Padding), LESSER_GU, LESSER_GU };
		designerDingBox = { dingSelectorDims.x + 2 + LESSER_GU + Padding, dingSelectorDims.y + 2, 6 * LESSER_GU + 5 * Padding, 5 * LESSER_GU + 4 * Padding };
		SDL_Rect moving = { dingSelectorDims.x + dingSelectorDims.w - 2 - 1 * (2 * LESSER_GU + Padding), dingSelectorDims.y + dingSelectorDims.h - 2 - 2 * LESSER_GU - Padding, 2 * LESSER_GU + Padding, 2 * LESSER_GU + Padding};
		SDL_Rect roting = { dingSelectorDims.x + dingSelectorDims.w - 2 - 2 * (2 * LESSER_GU + Padding) - Padding, dingSelectorDims.y + dingSelectorDims.h - 2 - 2 * LESSER_GU - Padding, 2 * LESSER_GU + Padding, 2 * LESSER_GU + Padding};
		SDL_Rect splode = { dingSelectorDims.x + dingSelectorDims.w - 2 - 3 * (2 * LESSER_GU + Padding) - 2 * Padding, dingSelectorDims.y + dingSelectorDims.h - 2 - 2 * LESSER_GU - Padding, 2 * LESSER_GU + Padding, 2 * LESSER_GU + Padding };

		cairo_rectangle(canvas, biome1.x, biome1.y, biome1.w, biome1.h);
		cairo_rectangle(canvas, biome2.x, biome2.y, biome2.w, biome2.h);
		cairo_rectangle(canvas, biome3.x, biome3.y, biome3.w, biome3.h);
		cairo_rectangle(canvas, biome4.x, biome4.y, biome4.w, biome4.h);
		cairo_rectangle(canvas, biome5.x, biome5.y, biome5.w, biome5.h);
		cairo_rectangle(canvas, biome6.x, biome6.y, biome6.w, biome6.h);
		cairo_rectangle(canvas, biome7.x, biome7.y, biome7.w, biome7.h);
		cairo_rectangle(canvas, designerDingBox.x, designerDingBox.y, designerDingBox.w, designerDingBox.h);
		cairo_rectangle(canvas, moving.x, moving.y, moving.w, moving.h);
		cairo_rectangle(canvas, roting.x, roting.y, roting.w, roting.h);
		cairo_rectangle(canvas, splode.x, splode.y, splode.w, splode.h);
		cairo_stroke(canvas);

		EndRenderDrawing(GameViewRenderer, canvasTexture, nullptr);

		// X  XXXXXXXXXXXXXXXXXXXXXXX
		//    X                     X
		// X  X                     X
		//    X                     X
		// X  X                     X
		//    X                     X
		// X  X                     X
		//    X                     X
		// X  X                     X
		//    X                     X
		// X  XXXXXXXXXXXXXXXXXXXXXXX
		//
		// X  X          X          X
		//
		// left column = biome preset
		// bottom row = move/rotate/remove tool

		const auto dingLocator = GetDing(Ding::BarrelLoaded);
		if (dingLocator->onSheet)
		{
			SDL_Rect dst = {
				splode.x + splode.w / 2 - dingLocator->src.w / 2,
				splode.y + splode.h / 2 - dingLocator->src.h / 2,
				dingLocator->src.w,
				dingLocator->src.h
			};

			if (SDL_RenderCopy(
				GameViewRenderer,
				dingLocator->onSheet,
				&dingLocator->src,
				&dst
			) != 0)
			{
				const auto barrelError = SDL_GetError();
				ReportError("Failed to place the splode barrel on ding selector tool", barrelError);

				return false;
			};
		}

		if (SDL_SetRenderTarget(GameViewRenderer, NULL) < 0)
		{
			const auto resetError = SDL_GetError();
			ReportError("Failed to reset the render target after rendering to ding selector tool texture", resetError);

			return false;
		}

		return true;
	}

	/// <summary>
	/// The gridlock and coalesce mode switch controls
	/// are also considered part of the design layers tool
	/// </summary>
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
		auto const& yStride = designLayersDims.w / 4.;
		auto const& xExtent = designLayersDims.w / 3.;
		auto const& xSlantExtent = designLayersDims.w / 4.;
		auto const& yExtent = designLayersDims.w / 6;
		auto const& modeHeight = 120;
		double const& baseY = designLayersDims.y + designLayersDims.h - modeHeight;
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
				blooDot::MakerControls::DrawSelectedLayerTool(canvas, designLayersDims.x, baseY, yStride, xExtent, xSlantExtent, yExtent, widthPadding * 2, etage);
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

		auto gridlock = [canvas, dashWidth](double angle, double x, BitwiseTileIndex edgesFaces)
		{
			cairo_matrix_t matrix1, matrix2, matrix3, matrix4, matrix5, matrix6, matrix7;
			
			cairo_push_group(canvas);
			cairo_matrix_init_translate(&matrix1, -GRIDUNIT / 2., -GRIDUNIT / 2.);
			cairo_matrix_init_rotate(&matrix2, angle);
			cairo_matrix_init_translate(&matrix3, 2.5 * (x + GRIDUNIT / 2. + dashWidth), 2.5 * (GRIDUNIT / 2. + designLayersDest.h - 2 * GRIDUNIT));
			cairo_matrix_init_scale(&matrix4, .4, .4);
			cairo_matrix_multiply(&matrix5, &matrix1, &matrix2);
			cairo_matrix_multiply(&matrix6, &matrix5, &matrix3);
			cairo_matrix_multiply(&matrix7, &matrix6, &matrix4);
			cairo_set_matrix(canvas, &matrix7);
			blooDot::Dings::DrawDing(canvas, Ding::WallClassic, edgesFaces);
			cairo_pop_group_to_source(canvas);
			cairo_paint(canvas);
		};

		SDL_Texture* labelTexture = nullptr;
		if (isGridlockOn)
		{
			gridlock(0, 0, isCoalesceOn ? BitwiseTileIndex::WestEdge | BitwiseTileIndex::NorthEdge | BitwiseTileIndex::SouthEdge : BitwiseTileIndex::Bung);
			gridlock(0, GRIDUNIT * .4, isCoalesceOn ? BitwiseTileIndex::EastEdge | BitwiseTileIndex::NorthEdge | BitwiseTileIndex::SouthEdge : BitwiseTileIndex::Bung);
			labelTexture = gridlockLabelOn;
		}
		else
		{
			gridlock(.9, 0, BitwiseTileIndex::Bung);
			gridlock(-.21, GRIDUNIT * .55, BitwiseTileIndex::Bung);
			labelTexture = gridlockLabel;
		}

		auto const& gridlockLabelPos = SDL_Rect
		{
			designLayersDims.x + designLayersDims.w - gridlockLabelRect.w,
			static_cast<int>(designLayersDest.h - 1.3 * GRIDUNIT - gridlockLabelRect.h),
			gridlockLabelRect.w,
			gridlockLabelRect.h
		};

		SDL_RenderCopy(GameViewRenderer, labelTexture, NULL, &gridlockLabelPos);
		auto coalesce = [canvas, dashWidth](ClumsyPack::BitwiseTileIndex edgesFaces, double x, double y)
		{
			cairo_matrix_t matrix1, matrix2, matrix3, matrix4, matrix5;
			cairo_push_group(canvas);
			cairo_matrix_init_translate(&matrix1, -GRIDUNIT / 2., -GRIDUNIT / 2.);
			cairo_matrix_init_translate(&matrix2, 2.5 * (x + GRIDUNIT / 2. - dashWidth), 2.5 * (y + designLayersDest.h - .4 * GRIDUNIT));
			cairo_matrix_init_scale(&matrix3, .4, .4);
			cairo_matrix_multiply(&matrix4, &matrix1, &matrix2);
			cairo_matrix_multiply(&matrix5, &matrix4, &matrix3);
			cairo_set_matrix(canvas, &matrix5);
			blooDot::Dings::DrawDing(canvas, Ding::WallClassic, edgesFaces);
			cairo_pop_group_to_source(canvas);
			cairo_paint(canvas);
		};

		if (isCoalesceOn)
		{
			labelTexture = coalesceLabelOn;
			coalesce(ClumsyPack::WestEdge | ClumsyPack::NorthEdge | ClumsyPack::SouthEdge, 0 + GRIDUNIT * .4 * 0, 0);
			coalesce(ClumsyPack::SouthEdge | ClumsyPack::NorthWestCorner | ClumsyPack::NorthEastCorner, 0 + GRIDUNIT * .4 * 1, 0);
			coalesce(ClumsyPack::NorthEdge | ClumsyPack::WestEdge | ClumsyPack::EastEdge, 0 + GRIDUNIT * .4 * 1, -GRIDUNIT * .4);
			coalesce(ClumsyPack::EastEdge | ClumsyPack::NorthEdge | ClumsyPack::SouthEdge, 0 + GRIDUNIT * .4 * 2, 0);
		}
		else
		{
			labelTexture = coalesceLabel;
			coalesce(ClumsyPack::Bung, 0 + GRIDUNIT * .4 * 0, 0);
			coalesce(ClumsyPack::Bung, 0 + GRIDUNIT * .4 * 1, 0);
			coalesce(ClumsyPack::Bung, 0 + GRIDUNIT * .4 * 1, -GRIDUNIT * .4);
			coalesce(ClumsyPack::Bung, 0 + GRIDUNIT * .4 * 2, 0);
		}

		auto const& coalesceLabelPos = SDL_Rect
		{
			designLayersDims.x + designLayersDims.w - gridlockLabelRect.w,
			static_cast<int>(designLayersDest.h - .4 * GRIDUNIT - gridlockLabelRect.h),
			gridlockLabelRect.w,
			gridlockLabelRect.h
		};

		SDL_RenderCopy(GameViewRenderer, labelTexture, NULL, &coalesceLabelPos);
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