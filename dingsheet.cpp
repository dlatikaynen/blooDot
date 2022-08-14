#include "pch.h"
#include "dingsheet.h"

extern SDL_Renderer* GameViewRenderer;

std::vector<SDL_Texture*> dingSheets;
std::map<Ding, DingLocator> dingMap;

DingLocator* GetDing(Ding ding)
{
	const auto lookedUp = dingMap.find(ding);
	
	if (lookedUp == dingMap.end())
	{
		/* not already on a sheet, load-on-demand now */
		SDL_Rect dingDimensions;
		const auto resourceKey = _GetDingResourceKey(ding);
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
	for (auto const& dingSheet : dingSheets)
	{
		SDL_DestroyTexture(dingSheet);
	}
}

int _GetDingResourceKey(Ding ding)
{
	switch (ding)
	{
	case Ding::BarrelIndigo:
		return CHUNK_KEY_DINGS_BARREL_INDIGO;

	case Ding::FloorSlate:
		return CHUNK_KEY_DINGS_FLOORSTONETILE_SLATE;

	default:
		return -1;
	}
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
		placeOnSheet = SDL_CreateTexture(
			GameViewRenderer, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET,
			800,
			600
		);

		if (!placeOnSheet)
		{
			const auto newSheetError = SDL_GetError();
			ReportError("Could not allocate a new ding sheet", newSheetError);
			return {};
		}

		dingSheets.push_back(placeOnSheet);
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

	(*frame).x = 0;
	(*frame).y = 0;

	return true;
}
