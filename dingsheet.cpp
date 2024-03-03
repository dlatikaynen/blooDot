#include "pch.h"
#include "dingsheet.h"
#include <iostream>
#include "drawing.h"

constexpr const int DING_SHEET_SIDE_LENGTH = 800;

extern SDL_Renderer* GameViewRenderer;

std::vector<SDL_Texture*> dingSheets;
std::map<Ding, DingLocator> dingMap;
std::vector<SDL_Rect> occupied; // bottom-left rectangle packing is good enough

DingLocator* GetDing(Ding ding)
{
	const auto lookedUp = dingMap.find(ding);
	
	if (lookedUp == dingMap.end())
	{
		/* not already on a sheet, load-on-demand now */
		SDL_Rect dingDimensions;
		SDL_Texture* dingTexture = NULL;

		if (blooDot::Dings::IsOwnerDrawnDing(ding))
		{
			dingTexture = _OwnerDrawDing(ding, &dingDimensions);
		}
		else
		{
			const auto resourceKey = blooDot::Dings::GetDingResourceKey(ding);
			dingTexture = _LoadDingTexture(resourceKey, &dingDimensions);
		}
		
		if (!dingTexture)
		{
			return NULL;
		}

		const auto newLocator = _PlaceOnDingSheet(&dingDimensions, dingTexture, ding);
		SDL_DestroyTexture(dingTexture);
		dingMap[ding] = newLocator;

		return &dingMap[ding];
	}
	else
	{
		return &((*lookedUp).second);
	}	
}

void TeardownDingSheets()
{
	occupied.clear();
	dingMap.clear();
	for (auto const& dingSheet : dingSheets)
	{
		SDL_DestroyTexture(dingSheet);
	}

	dingSheets.clear();
}

SDL_Texture* _LoadDingTexture(int resourceKey, __out SDL_Rect* dimensions)
{
	*dimensions = { 0,0,0,0 };

	SDL_RWops* resStream;
	const auto resMem = Retrieve(resourceKey, &resStream);
	if (!resMem)
	{
		return NULL;
	}

	const auto resPicture = IMG_LoadPNG_RW(resStream);
	(*dimensions).w = resPicture->w;
	(*dimensions).h = resPicture->h;
	resStream->close(resStream);
	SDL_free(resMem);
	if (!resPicture)
	{
		const auto resLoadError = IMG_GetError();
		ReportError("Failed to load ding resource", resLoadError);
		return NULL;
	}

	const auto resTexture = SDL_CreateTextureFromSurface(GameViewRenderer, resPicture);
	SDL_free(resPicture);
	if (!resTexture)
	{
		const auto textureError = SDL_GetError();
		ReportError("Failed to load ding texture", textureError);
		return NULL;
	}

	return resTexture;
}

SDL_Texture* _OwnerDrawDing(Ding ding, __out SDL_Rect* dimensions)
{
	auto& bounds = (*dimensions);
	bounds = { 0,0,GRIDUNIT,GRIDUNIT };
	auto drawTo = NewTexture(GameViewRenderer, GRIDUNIT, GRIDUNIT, true);
	if (drawTo == NULL)
	{
		return NULL;
	}

	auto restoreTarget = SDL_GetRenderTarget(GameViewRenderer);
	if(SDL_SetRenderTarget(GameViewRenderer, drawTo) < 0)
	{
		const auto targetError = SDL_GetError();
		ReportError("Could not set rendertarget for ding ownerdraw", targetError);
		return NULL;
	}

	auto canvasTexture = BeginRenderDrawing(GameViewRenderer, GRIDUNIT, GRIDUNIT);
	auto const& canvas = GetDrawingSink();
	blooDot::Dings::DrawDing(canvas, ding);
	EndRenderDrawing(GameViewRenderer, canvasTexture, nullptr);
	if (SDL_SetRenderTarget(GameViewRenderer, restoreTarget) < 0)
	{
		const auto restoreError = SDL_GetError();
		ReportError("Could not restore rendertarget after ding ownerdraw", restoreError);
		return NULL;
	}

	return drawTo;
}

DingLocator _PlaceOnDingSheet(SDL_Rect* dingDimensions, SDL_Texture* dingTexture, Ding dingInfo)
{
	DingLocator newLocator{};
	SDL_Rect finalSrc{};
	SDL_Texture* placeOnSheet = NULL;
	if (!dingSheets.empty())
	{
		placeOnSheet = dingSheets.back();
	}

	finalSrc.w = dingDimensions->w;
	finalSrc.h = dingDimensions->h;

	if (!_FindRoomOnDingSheet(placeOnSheet, &finalSrc, dingInfo))
	{
		occupied.clear();
		placeOnSheet = SDL_CreateTexture(
			GameViewRenderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET,
			DING_SHEET_SIDE_LENGTH,
			DING_SHEET_SIDE_LENGTH
		);

		if (!placeOnSheet)
		{
			const auto newSheetError = SDL_GetError();
			ReportError("Could not allocate a new ding sheet", newSheetError);
			return {};
		}

		dingSheets.push_back(placeOnSheet);
		occupied.push_back(finalSrc);
		SDL_SetTextureBlendMode(placeOnSheet, SDL_BLENDMODE_BLEND);
	}

	auto previousTarget = SDL_GetRenderTarget(GameViewRenderer);

	if (SDL_SetRenderTarget(GameViewRenderer, placeOnSheet) < 0)
	{
		const auto targetError = SDL_GetError();
		ReportError("Could not set ding sheet as the render target", targetError);
		return {};
	}

	if (SDL_RenderCopy(GameViewRenderer, dingTexture, dingDimensions, &finalSrc) < 0)
	{
		const auto copyError = SDL_GetError();
		ReportError("Could not copy ding onto ding sheet", copyError);
		return {};
	}

	if (SDL_SetRenderTarget(GameViewRenderer, previousTarget) < 0)
	{
		const auto restoreError = SDL_GetError();
		ReportError("Could not restore render target after rendering to ding sheet", restoreError);
		return {};
	}

	newLocator.onSheet = placeOnSheet;
	newLocator.src = finalSrc;
	return newLocator;
}

bool _FindRoomOnDingSheet(SDL_Texture* sheet, SDL_Rect* frame, Ding dingInfo)
{
	if (!sheet)
	{
		return false;
	}

	assert(!occupied.empty());
	auto& curLine = occupied.back();
	if (curLine.y + frame->h > DING_SHEET_SIDE_LENGTH)
	{
		/* does not fit height-wise */
		std::stringstream howBigIsTooBig;
		howBigIsTooBig << "Been asked to allocate " << frame->h << " high for " << dingInfo << ", but the current line is " << curLine.h << " and maxes out at " << DING_SHEET_SIDE_LENGTH - curLine.y << " right now";
		const auto& strHowBigIsTooBig = howBigIsTooBig.str();

		ReportError("Ding too high for sheet", strHowBigIsTooBig.c_str());
		
		return false;
	}

	if (frame->w > DING_SHEET_SIDE_LENGTH)
	{
		/* does not fit at all, not even on a new line */
		std::stringstream howBigIsTooBig;
		howBigIsTooBig << "Been asked to allocate " << frame->w << " wide for " << dingInfo << ", but am only " << DING_SHEET_SIDE_LENGTH << " wide altogether";
		const auto& strHowBigIsTooBig = howBigIsTooBig.str();
		
		ReportError("Ding too big for sheet", strHowBigIsTooBig.c_str());
		return false;
	}

	if (curLine.w + frame->w > DING_SHEET_SIDE_LENGTH)
	{
		/* does not fit across, attempt to allocate a new line once */
		SDL_Rect newLine{ 0,curLine.y + curLine.h,0,frame->h };
		if (newLine.y + newLine.h > DING_SHEET_SIDE_LENGTH)
		{
			std::stringstream howBigIsTooBig;
			howBigIsTooBig << "Been asked to allocate " << frame->h << " high for " << dingInfo << ", but the highest possible line at " << (newLine.y + newLine.h) << " would exceed the available " << DING_SHEET_SIDE_LENGTH << " of sheet height";
			const auto& strHowBigIsTooBig = howBigIsTooBig.str();
			
			ReportError("Ding too high for remaining sheet", strHowBigIsTooBig.c_str());
			
			return false;
		}
		
		occupied.push_back(newLine);
		curLine = occupied.back();
	}

	curLine.h = frame->h > curLine.h ? frame->h : curLine.h;
	(*frame).x = curLine.w;
	(*frame).y = curLine.y;
	curLine.w += frame->w;
	occupied.pop_back();
	occupied.push_back(curLine);

#ifndef NDEBUG
	std::cout
		<< "Allocated ("
		<< frame->x
		<< ","
		<< frame->y
		<< ","
		<< frame->w
		<< ","
		<< frame->h
		<< ") on the "
		<< sheet
		<< " sheet's line number "
		<< occupied.size()
		<< " for "
		<< blooDot::Dings::GetDingName(dingInfo)
		<< "\n";
#endif

	return true;
}
