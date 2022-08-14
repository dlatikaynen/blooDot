#include "pch.h"
#include "flap.h"

extern unsigned short flapIndirection[9];
extern SDL_Texture* flapsFloor[9];
extern SDL_Texture* flapsWalls[9];
extern SDL_Texture* flapsRooof[9];
extern SDL_Renderer* GameViewRenderer;
extern int flapW;
extern int flapH;
extern int halfW;
extern int halfH;

void PopulateFlap(int flapIndex)
{
	const auto textureIndex = flapIndirection[flapIndex];
	const auto floor = flapsFloor[textureIndex];
	const auto walls = flapsWalls[textureIndex];
	const auto rooof = flapsRooof[textureIndex];

	/* where in the world are we? */
	const auto leftOfMidpoint = -GRIDUNIT / 2;
	const auto nrthofMidpoint = -GRIDUNIT / 2;
	const auto leftStart = leftOfMidpoint - (halfW / GRIDUNIT + 1) * GRIDUNIT;
	const auto nrthStart = nrthofMidpoint - (halfH / GRIDUNIT + 1) * GRIDUNIT;
	const auto gridUnitsPerRow = (int)ceil((double)flapW / GRIDUNIT);
	const auto gridUnitsPerCol = (int)ceil((double)flapH / GRIDUNIT);
	const auto leftWorldY = -gridUnitsPerRow / 2;
	auto worldX = leftWorldY;
	auto worldY = -gridUnitsPerCol / 2;

	std::cout << leftOfMidpoint << "," << leftStart << "," << gridUnitsPerRow << "\n";

	//auto drawingSink = BeginTextureDrawing(floor);
	bool isFirst = true;
	int lastIterX = 0, lastIterY = 0;
	for (auto y = halfH + nrthStart; y <= flapH; y += GRIDUNIT)
	{
		for (auto x = halfW + leftStart; x <= flapW; x += GRIDUNIT)
		{
			if (isFirst)
			{
				//cairo_set_source_rgb(drawingSink, 0, 1, 1);
				isFirst = false;
			}
			else
			{
				//cairo_set_source_rgb(drawingSink, 0, 0, 1);
				lastIterX = x;
				lastIterY = y;
			}

			//cairo_move_to(drawingSink, x + 1, y);
			//cairo_line_to(drawingSink, x + 4, y);
			//cairo_move_to(drawingSink, x, y + 1);
			//cairo_line_to(drawingSink, x, y + 4);

			//cairo_move_to(drawingSink, x + GRIDUNIT - 1, y + GRIDUNIT);
			//cairo_line_to(drawingSink, x + GRIDUNIT - 4, y + GRIDUNIT);
			//cairo_move_to(drawingSink, x + GRIDUNIT, y + GRIDUNIT - 1);
			//cairo_line_to(drawingSink, x + GRIDUNIT, y + GRIDUNIT - 4);
			//cairo_stroke(drawingSink);

			const auto cellPiece = GetPieceRelative(worldX, worldY);
			if (!cellPiece.dings.empty())
			{
				for (auto& ding : cellPiece.dings)
				{
					const auto dingLocator = GetDing(ding.ding);
					if (dingLocator->onSheet)
					{
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
							ReportError("Could not place ding on dingsheet", placeError);
						}

						if (SDL_SetRenderTarget(GameViewRenderer, NULL) < 0)
						{
							const auto restoreError = SDL_GetError();
							ReportError("Could not restore render target", restoreError);
						}
					}
				}
			}

			++worldX;
		}

		++worldY;
		worldX = leftWorldY;
	}

	/*
	cairo_set_source_rgb(drawingSink, 0, 1, 1);
	cairo_move_to(drawingSink, lastIterX + 1, lastIterY);
	cairo_line_to(drawingSink, lastIterX + 4, lastIterY);
	cairo_move_to(drawingSink, lastIterX, lastIterY + 1);
	cairo_line_to(drawingSink, lastIterX, lastIterY + 4);
	cairo_move_to(drawingSink, lastIterX + GRIDUNIT - 1, lastIterY + GRIDUNIT);
	cairo_line_to(drawingSink, lastIterX + GRIDUNIT - 4, lastIterY + GRIDUNIT);
	cairo_move_to(drawingSink, lastIterX + GRIDUNIT, lastIterY + GRIDUNIT - 1);
	cairo_line_to(drawingSink, lastIterX + GRIDUNIT, lastIterY + GRIDUNIT - 4);
	cairo_stroke(drawingSink);

#ifndef NDEBUG
	cairo_set_source_rgb(drawingSink, 1, 0.68, 0.08);

	cairo_move_to(drawingSink, 0, 16);
	cairo_line_to(drawingSink, 0, 0);
	cairo_line_to(drawingSink, 16, 0);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, flapW - 16, 0);
	cairo_line_to(drawingSink, flapW, 0);
	cairo_line_to(drawingSink, flapW, 16);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, flapW - 16, flapH);
	cairo_line_to(drawingSink, flapW, flapH);
	cairo_line_to(drawingSink, flapW, flapH - 16);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, 0, flapH - 16);
	cairo_line_to(drawingSink, 0, flapH);
	cairo_line_to(drawingSink, 16, flapH);
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

#endif
	EndTextureDrawing(floor, drawingSink);

#ifndef NDEBUG
	drawingSink = BeginTextureDrawing(walls);

	cairo_set_source_rgb(drawingSink, 0.68, 1, 0.08);

	cairo_move_to(drawingSink, 1, 15);
	cairo_line_to(drawingSink, 1, 1);
	cairo_line_to(drawingSink, 15, 1);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, flapW - 15, 1);
	cairo_line_to(drawingSink, flapW - 1, 1);
	cairo_line_to(drawingSink, flapW - 1, 15);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, flapW - 16, flapH - 1);
	cairo_line_to(drawingSink, flapW - 1, flapH - 1);
	cairo_line_to(drawingSink, flapW - 1, flapH - 15);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, 1, flapH - 15);
	cairo_line_to(drawingSink, 1, flapH - 1);
	cairo_line_to(drawingSink, 15, flapH - 1);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, 20, 26);
	cairo_text_path(drawingSink, std::to_string(flapIndex).c_str());
	cairo_move_to(drawingSink, 35, 26);
	cairo_text_path(drawingSink, std::to_string(textureIndex).c_str());

	cairo_fill(drawingSink);

	EndTextureDrawing(walls, drawingSink);
#endif

#ifndef NDEBUG
	drawingSink = BeginTextureDrawing(rooof);

	cairo_set_source_rgb(drawingSink, 0.08, 0.68, 1);

	cairo_move_to(drawingSink, 2, 14);
	cairo_line_to(drawingSink, 2, 2);
	cairo_line_to(drawingSink, 14, 2);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, flapW - 14, 2);
	cairo_line_to(drawingSink, flapW - 2, 2);
	cairo_line_to(drawingSink, flapW - 2, 14);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, flapW - 14, flapH - 2);
	cairo_line_to(drawingSink, flapW - 2, flapH - 2);
	cairo_line_to(drawingSink, flapW - 2, flapH - 14);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, 2, flapH - 14);
	cairo_line_to(drawingSink, 2, flapH - 2);
	cairo_line_to(drawingSink, 14, flapH - 2);
	cairo_stroke(drawingSink);

	cairo_move_to(drawingSink, 30, 36);
	cairo_text_path(drawingSink, std::to_string(flapIndex).c_str());
	cairo_move_to(drawingSink, 45, 36);
	cairo_text_path(drawingSink, std::to_string(textureIndex).c_str());

	cairo_fill(drawingSink);

	EndTextureDrawing(rooof, drawingSink);
#endif
*/
}
