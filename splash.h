#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>

extern void ReportError(const char*, const char*);
extern void* Retrieve(int chunkKey, __out SDL_RWops** const stream);
extern bool LoadFonts();
extern TTF_Font* GetFont(int fontKey);
extern SDL_Texture* RenderText(SDL_Renderer*, SDL_Rect*, int, int, const char*, SDL_Color, bool bold);
extern SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight);
extern cairo_t* GetDrawingSink();
extern void EndRenderDrawing(SDL_Renderer* renderTarget, SDL_Texture* targetTexture, SDL_Rect* destRect);
extern void DrawLabel(SDL_Renderer*, int, int, SDL_Texture*, SDL_Rect*);
extern cairo_t* DrawChevron(cairo_t*, double, double, bool, unsigned short);
extern bool SettingsMenuLoop(SDL_Renderer*);

namespace blooDot::Splash
{
	bool SplashLoop(SDL_Renderer*);

	void _Bounce(SDL_Rect* srcRect);
	void _AssignNewSpeed(__out int* speed);
	void _DelayBackgroundAnim();
	void _PrepareText(SDL_Renderer* renderer, bool destroy = false);
	bool _HandleLaunch(SDL_Renderer* renderer, bool stayInMenu);
	bool _EnterAndHandleMenu(SDL_Renderer* renderer);
	void _EnterAndHandleCreatorMode(SDL_Renderer* renderer);
	void _EnterAndHandleSettings(SDL_Renderer* renderer);
}