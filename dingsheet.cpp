#include "pch.h"
#include "dingsheet.h"

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
		const auto resourceKey = GetDingResourceKey(ding);
		const auto dingTexture = _LoadDingTexture(resourceKey, &dingDimensions);
		if (!dingTexture)
		{
			return NULL;
		}
		
		const auto newLocator = _PlaceOnDingSheet(&dingDimensions, dingTexture);
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

DingLocator _PlaceOnDingSheet(SDL_Rect* dingDimensions, SDL_Texture* dingTexture)
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

	if (!_FindRoomOnDingSheet(placeOnSheet, &finalSrc))
	{
		occupied.clear();
		placeOnSheet = SDL_CreateTexture(
			GameViewRenderer, SDL_PIXELFORMAT_ARGB8888,
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

	if (SDL_SetRenderTarget(GameViewRenderer, NULL) < 0)
	{
		const auto restoreError = SDL_GetError();
		ReportError("Could restore render target after rendering to ding sheet", restoreError);
		return {};
	}

	newLocator.onSheet = placeOnSheet;
	newLocator.src = finalSrc;
	return newLocator;
}

bool _FindRoomOnDingSheet(SDL_Texture* sheet, SDL_Rect* frame)
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
		return false;
	}

	if (curLine.w + frame->w > DING_SHEET_SIDE_LENGTH)
	{
		/* does not fit across */
		return false;
	}

	curLine.h = frame->h > curLine.h ? frame->h : curLine.h;
	(*frame).x = curLine.w;
	(*frame).y = curLine.y;
	curLine.w += frame->w;
	occupied.pop_back();
	occupied.push_back(curLine);

	return true;
}
