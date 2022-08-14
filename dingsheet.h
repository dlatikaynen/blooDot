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

void TeardownDingSheets();
DingLocator* GetDing(Ding ding);

int _GetDingResourceKey(Ding ding);
SDL_Texture* _LoadDingTexture(int resourceKey, __out SDL_Rect* dimensions);
DingLocator _PlaceOnDingSheet(SDL_Rect* dingDimensions, SDL_Texture* dingTexture);
bool _FindRoomOnDingSheet(SDL_Texture* sheet, SDL_Rect* frame);