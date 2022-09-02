#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "ding.h"
#include "dexassy.h"

typedef struct DingLocator {
	SDL_Texture* onSheet;
	SDL_Rect src;
} DingLocator;

extern int GetDingResourceKey(Ding);
extern void DrawDing(const Ding ding, cairo_t* canvas);

void TeardownDingSheets();
DingLocator* GetDing(Ding ding);

SDL_Texture* _LoadDingTexture(int resourceKey, __out SDL_Rect* dimensions);
SDL_Texture* _OwnerDrawDing(Ding ding, __out SDL_Rect* dimensions);
DingLocator _PlaceOnDingSheet(SDL_Rect* dingDimensions, SDL_Texture* dingTexture, Ding dingInfo);
bool _FindRoomOnDingSheet(SDL_Texture* sheet, SDL_Rect* frame, Ding dingInfo);