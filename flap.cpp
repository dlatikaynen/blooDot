#include "pch.h"
#include "flap.h"

extern unsigned short flapIndirection[9];
extern SDL_Texture* flapsFloor[9];
extern SDL_Texture* flapsWalls[9];
extern SDL_Texture* flapsRooof[9];
#ifndef NDEBUG
extern SDL_Texture* debugFlap;
extern SDL_Rect bunghole;
extern bool toggleDebugView;
#endif
extern SDL_Renderer* GameViewRenderer;
extern int flapW;
extern int flapH;
extern int halfW;
extern int halfH;

FlapAwareness flapAwareness[9]{};

void PopulateFlap(int flapIndex, int flapInWorldX, int flapInWorldY)
{
	const auto textureIndex = flapIndirection[flapIndex];
	const auto floor = flapsFloor[textureIndex];
	const auto walls = flapsWalls[textureIndex];
	const auto rooof = flapsRooof[textureIndex];

	/* where in the world are we? */
	const auto myAwareness = _FigureOutAwarenessFor(flapIndex, flapInWorldX, flapInWorldY);
	const auto leftWorldX = myAwareness->myGridLeftX;
	auto worldX = leftWorldX;
	auto worldY = myAwareness->myGridToopY;

	if (SDL_SetRenderTarget(GameViewRenderer, floor) < 0)
	{
		const auto floorError = SDL_GetError();
		ReportError("Could not clear floor render target", floorError);
	}

	SDL_SetRenderDrawColor(GameViewRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(GameViewRenderer);
	if (SDL_SetRenderTarget(GameViewRenderer, walls) < 0)
	{
		const auto wallsError = SDL_GetError();
		ReportError("Could not clear walls render target", wallsError);
	}

	SDL_SetRenderDrawColor(GameViewRenderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
	SDL_RenderClear(GameViewRenderer);
	if (SDL_SetRenderTarget(GameViewRenderer, rooof) < 0)
	{
		const auto rooofError = SDL_GetError();
		ReportError("Could not clear rooof render target", rooofError);
	}

	SDL_SetRenderDrawColor(GameViewRenderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
	SDL_RenderClear(GameViewRenderer);

#ifndef NDEBUG
	const auto drawingSink = BeginTextureDrawing(debugFlap, flapW, flapH);
	cairo_set_source_rgba(drawingSink, 0, 0, 0, 0);
	cairo_rectangle(drawingSink, 0, 0, flapW, flapH);
	cairo_fill(drawingSink);
	cairo_set_operator(drawingSink, CAIRO_OPERATOR_SOURCE);
	cairo_paint_with_alpha(drawingSink, 1);
	bool isFirst = true;
	int lastIterX = 0, lastIterY = 0;
	int objectCount = 0;
#endif

	for (auto y = myAwareness->localDrawingOffsetY; y <= flapH; y += GRIDUNIT)
	{
		for (auto x = myAwareness->localDrawingOffsetX; x <= flapW; x += GRIDUNIT)
		{
#ifndef NDEBUG
			if (isFirst)
			{
				cairo_set_source_rgb(drawingSink, 1, 1, 1);
				cairo_move_to(drawingSink, x + GRIDUNIT + 3, y + GRIDUNIT + 13);
				cairo_set_font_size(drawingSink, 16);
				std::stringstream flapInWorldLabel;
				flapInWorldLabel << "[" << myAwareness->myGridLeftX << "," << myAwareness->myGridToopY << "]";					
				cairo_text_path(drawingSink, flapInWorldLabel.str().c_str());
				cairo_fill(drawingSink);
				cairo_set_source_rgb(drawingSink, 0, 1, 1);
				isFirst = false;
			}
			else
			{
				cairo_set_source_rgb(drawingSink, 0, 0, 1);
				lastIterX = x;
				lastIterY = y;
			}

			cairo_move_to(drawingSink, x + 1, y);
			cairo_line_to(drawingSink, x + 4, y);
			cairo_move_to(drawingSink, x, y + 1);
			cairo_line_to(drawingSink, x, y + 4);

			cairo_move_to(drawingSink, x + GRIDUNIT - 1, y + GRIDUNIT);
			cairo_line_to(drawingSink, x + GRIDUNIT - 4, y + GRIDUNIT);
			cairo_move_to(drawingSink, x + GRIDUNIT, y + GRIDUNIT - 1);
			cairo_line_to(drawingSink, x + GRIDUNIT, y + GRIDUNIT - 4);
			cairo_stroke(drawingSink);
#endif

			const auto cellPiece = GetPieceRelative(worldX, worldY);
			if (!cellPiece.dings.empty())
			{
				for (auto& ding : cellPiece.dings)
				{
					const auto dingLocator = GetDing(ding.ding);
					if (dingLocator->onSheet) [[likely]]
					{
#ifndef NDEBUG
						++objectCount;
#endif
						if (ding.props & DingProps::Walls)
						{
							if (SDL_SetRenderTarget(GameViewRenderer, walls) < 0)
							{
								const auto wallsError = SDL_GetError();
								ReportError("Could not set render target to walls", wallsError);
							}
						}
						else if (ding.props & DingProps::Rooof)
						{
							if (SDL_SetRenderTarget(GameViewRenderer, rooof) < 0)
							{
								const auto rooofError = SDL_GetError();
								ReportError("Could not set render target to rooof", rooofError);
							}
						}
						else
						{
							if (SDL_SetRenderTarget(GameViewRenderer, floor) < 0)
							{
								const auto floorError = SDL_GetError();
								ReportError("Could not set render target to floor", floorError);
							}
						}

						SDL_Rect dest = { x,y,GRIDUNIT,GRIDUNIT };
						if (SDL_RenderCopy(GameViewRenderer, dingLocator->onSheet, &(dingLocator->src), &dest) < 0)
						{
							const auto placeError = SDL_GetError();
							ReportError("Could not place ding on flap", placeError);
						}
					}
				}
			}

			++worldX;
		}

		++worldY;
		worldX = leftWorldX;
	}

#ifndef NDEBUG
	if (toggleDebugView)
	{
		cairo_set_source_rgb(drawingSink, 1, 0, 1);
		cairo_move_to(drawingSink, lastIterX + 1, lastIterY);
		cairo_line_to(drawingSink, lastIterX + 4, lastIterY);
		cairo_move_to(drawingSink, lastIterX, lastIterY + 1);
		cairo_line_to(drawingSink, lastIterX, lastIterY + 4);
		cairo_move_to(drawingSink, lastIterX + GRIDUNIT - 1, lastIterY + GRIDUNIT);
		cairo_line_to(drawingSink, lastIterX + GRIDUNIT - 4, lastIterY + GRIDUNIT);
		cairo_move_to(drawingSink, lastIterX + GRIDUNIT, lastIterY + GRIDUNIT - 1);
		cairo_line_to(drawingSink, lastIterX + GRIDUNIT, lastIterY + GRIDUNIT - 4);
		cairo_stroke(drawingSink);

		cairo_set_source_rgb(drawingSink, 1, 0.68, 0.08);

		cairo_move_to(drawingSink, 0, 13);
		cairo_line_to(drawingSink, 0, 0);
		cairo_line_to(drawingSink, 13, 0);
		cairo_stroke(drawingSink);

		cairo_move_to(drawingSink, flapW - 13, 0);
		cairo_line_to(drawingSink, flapW, 0);
		cairo_line_to(drawingSink, flapW, 13);
		cairo_stroke(drawingSink);

		cairo_move_to(drawingSink, flapW - 13, flapH);
		cairo_line_to(drawingSink, flapW, flapH);
		cairo_line_to(drawingSink, flapW, flapH - 13);
		cairo_stroke(drawingSink);

		cairo_move_to(drawingSink, 0, flapH - 13);
		cairo_line_to(drawingSink, 0, flapH);
		cairo_line_to(drawingSink, 13, flapH);
		cairo_stroke(drawingSink);

		const auto flapNr = std::to_string(flapIndex);
		const auto tureNr = std::to_string(textureIndex);
		cairo_move_to(drawingSink, 10, 16);
		cairo_text_path(drawingSink, flapNr.c_str());
		cairo_move_to(drawingSink, 25, 16);
		cairo_text_path(drawingSink, tureNr.c_str());

		cairo_move_to(drawingSink, flapW - 10 - 5, 16);
		cairo_text_path(drawingSink, flapNr.c_str());
		cairo_move_to(drawingSink, flapW - 25 - 5, 16);
		cairo_text_path(drawingSink, tureNr.c_str());

		cairo_move_to(drawingSink, flapW - 10 - 5, flapH - 16 + 8);
		cairo_text_path(drawingSink, flapNr.c_str());
		cairo_move_to(drawingSink, flapW - 25 - 5, flapH - 16 + 8);
		cairo_text_path(drawingSink, tureNr.c_str());

		cairo_move_to(drawingSink, 10, flapH - 16 + 8);
		cairo_text_path(drawingSink, flapNr.c_str());
		cairo_move_to(drawingSink, 25, flapH - 16 + 8);
		cairo_text_path(drawingSink, tureNr.c_str());

		cairo_fill(drawingSink);

		if (flapIndex == BUNGHOLE)
		{
			cairo_set_source_rgb(drawingSink, 0.28, 0.28, 0.28);
			cairo_move_to(drawingSink, halfW, halfH - 60);
			cairo_line_to(drawingSink, halfW, halfH + 60);
			cairo_move_to(drawingSink, halfW - 60, halfH);
			cairo_line_to(drawingSink, halfW + 60, halfH);
			cairo_stroke(drawingSink);
			cairo_set_source_rgb(drawingSink, 0.68, 0.68, 0.68);
			cairo_rectangle(drawingSink, halfW - GRIDUNIT / 2, halfH - GRIDUNIT / 2, GRIDUNIT, GRIDUNIT);
			cairo_stroke(drawingSink);
		}
	}

	EndTextureDrawing(debugFlap, drawingSink);
	if (toggleDebugView)
	{
		SDL_SetRenderTarget(GameViewRenderer, rooof);
		SDL_RenderCopy(GameViewRenderer, debugFlap, &bunghole, &bunghole);
	}

	std::cout
		<< "Populated flap #"
		<< flapIndex
		<< ", mapped to texture index "
		<< textureIndex
		<< " at world coordinates left "
		<< worldX
		<< ", top "
		<< worldY
		<< " with "
		<< objectCount
		<< "\n";
#endif

	if (SDL_SetRenderTarget(GameViewRenderer, NULL) < 0)
	{
		const auto restoreError = SDL_GetError();
		ReportError("Could not restore render target", restoreError);
	}
}

FlapAwareness* _FigureOutAwarenessFor(int flapIndex, int flapInWorldX, int flapInWorldY)
{
	const auto fictitiousFlapX = UnsignednessOffset + flapInWorldX;
	const auto fictitiousFlapY = UnsignednessOffset + flapInWorldY;

	// koordinate des rechten randes [5]
	const auto rightBorderX = flapW * (fictitiousFlapX + 1);
	const auto bottoBorderY = flapH * (fictitiousFlapY + 1);

#ifndef NDEBUG
	// koordinate des letzten quadrats [4]
	const auto lastGrindX = (ceil(rightBorderX / static_cast<double>(GRIDUNIT)) - 1) * GRIDUNIT;
	const auto lastGrindY = (ceil(bottoBorderY / static_cast<double>(GRIDUNIT)) - 1) * GRIDUNIT;

	// innenstand letztes quadrat lokal [6]
	const auto lastInnerX = rightBorderX - lastGrindX;
	const auto lastInnerY = bottoBorderY - lastGrindY;
#endif

	// anzahl mit überstand [7]
	const auto overCompensatedX = ceil(flapW / static_cast<double>(GRIDUNIT));
	const auto overCompensatedY = ceil(flapH / static_cast<double>(GRIDUNIT));

#ifndef NDEBUG
	// überstand letztes quadrat lokal [8]
	const auto lastOuterX = GRIDUNIT - lastInnerX;
	const auto lastOuterY = GRIDUNIT - lastInnerY;
#endif

	// koordinate des linken quadrats [9]
	const auto firstInnerX = (ceil((rightBorderX - static_cast<double>(flapW)) / static_cast<double>(GRIDUNIT)) - 1) * GRIDUNIT;
	const auto firstInnerY = (ceil((bottoBorderY - static_cast<double>(flapH)) / static_cast<double>(GRIDUNIT)) - 1) * GRIDUNIT;

	// koordinate des linken quadrats lokal [10]
	const auto firstLocalX = firstInnerX - (fictitiousFlapX * flapW);
	const auto firstLocalY = firstInnerY - (fictitiousFlapY * flapH);

#ifndef NDEBUG
	// innenstand des linken quadrats lokal [11]
	const auto firstLocalInnerX = GRIDUNIT + firstLocalX;
	const auto firstLocalInnerY = GRIDUNIT + firstLocalY;

	// summe innenstand und überstand erstes
	assert((firstLocalInnerX - firstLocalX) == GRIDUNIT);
	assert((firstLocalInnerY - firstLocalY) == GRIDUNIT);

	// summe innenstand und überstand letztes
	assert((lastInnerX + lastOuterX) == GRIDUNIT);
	assert((lastInnerY + lastOuterY) == GRIDUNIT);
#endif

	const auto gridInWorldX = ceil((firstInnerX - flapW * UnsignednessOffset) / static_cast<double>(GRIDUNIT) - overCompensatedX / 2);
	const auto gridInWorldY = ceil((firstInnerY - flapH * UnsignednessOffset) / static_cast<double>(GRIDUNIT) - overCompensatedY / 2);

	// both broadcast and return this for immediate use
	const auto awarenessEntry = &(flapAwareness[flapIndex]);
	awarenessEntry->flapInWorldX = flapInWorldX;
	awarenessEntry->flapInWorldY = flapInWorldY;
	awarenessEntry->localDrawingOffsetX = static_cast<int>(firstLocalX);
	awarenessEntry->localDrawingOffsetY = static_cast<int>(firstLocalY);
	awarenessEntry->numGridUnitsWide = static_cast<int>(overCompensatedX);
	awarenessEntry->numGridUnitsHigh = static_cast<int>(overCompensatedY);
	awarenessEntry->myGridLeftX = static_cast<int>(gridInWorldX);
	awarenessEntry->myGridToopY = static_cast<int>(gridInWorldY);

#ifndef NDEBUG
	std::cout
		<< "Flap #"
		<< flapIndex
		<< " describes ("
		<< flapInWorldX
		<< ","
		<< flapInWorldY
		<< "), is "
		<< overCompensatedX
		<< "gu wide, "
		<< overCompensatedY
		<< "gu high, represents ("
		<< gridInWorldX
		<< ","
		<< gridInWorldY
		<< ") and draws at ("
		<< firstLocalX
		<< ","
		<< firstLocalY
		<< ")\n";
#endif

	return awarenessEntry;
}

int FlapAwarenessLeft()
{
	flapAwareness[2] = flapAwareness[1];
	flapAwareness[5] = flapAwareness[4];
	flapAwareness[8] = flapAwareness[7];
	flapAwareness[1] = flapAwareness[0];
	flapAwareness[4] = flapAwareness[3];
	flapAwareness[7] = flapAwareness[6];

	/* they're going to be repopulated */
	flapAwareness[0] = {};
	flapAwareness[3] = {};
	flapAwareness[6] = {};

	/* we return where the new col shall be */
	return flapAwareness[4].flapInWorldX - 1;
}

int FlapAwarenessRite()
{
	flapAwareness[0] = flapAwareness[1];
	flapAwareness[3] = flapAwareness[4];
	flapAwareness[6] = flapAwareness[7];
	flapAwareness[1] = flapAwareness[2];
	flapAwareness[4] = flapAwareness[5];
	flapAwareness[7] = flapAwareness[8];

	/* they're going to be repopulated */
	flapAwareness[2] = {};
	flapAwareness[5] = {};
	flapAwareness[8] = {};

	/* we return where the new col shall be */
	return flapAwareness[4].flapInWorldX + 1;
}

int FlapAwarenessUuup()
{
	flapAwareness[6] = flapAwareness[3];
	flapAwareness[7] = flapAwareness[4];
	flapAwareness[8] = flapAwareness[5];
	flapAwareness[3] = flapAwareness[0];
	flapAwareness[4] = flapAwareness[1];
	flapAwareness[5] = flapAwareness[2];

	/* they're going to be repopulated */
	flapAwareness[0] = {};
	flapAwareness[1] = {};
	flapAwareness[2] = {};

	/* we return where the new row shall be */
	return flapAwareness[4].flapInWorldY - 1;
}

int FlapAwarenessDown()
{
	flapAwareness[0] = flapAwareness[3];
	flapAwareness[1] = flapAwareness[4];
	flapAwareness[2] = flapAwareness[5];
	flapAwareness[3] = flapAwareness[6];
	flapAwareness[4] = flapAwareness[7];
	flapAwareness[5] = flapAwareness[8];

	/* they're going to be repopulated */
	flapAwareness[6] = {};
	flapAwareness[7] = {};
	flapAwareness[8] = {};

	/* we return where the new row shall be */
	return flapAwareness[4].flapInWorldY + 1;
}

int GetCurrentBungCol()
{
	return flapAwareness[BUNGHOLE].flapInWorldX;
}

int GetCurrentBungRow()
{
	return flapAwareness[BUNGHOLE].flapInWorldY;
}
