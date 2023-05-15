#include "pch.h"
#include "region-banner.h"
#include "settings.h"
#include "drawing.h"
#include "xlations.h"

extern SDL_Renderer* GameViewRenderer;
extern SettingsStruct Settings;

namespace blooDot::RegionBanner
{
	constexpr Uint8 const BannerGray = 0x80;
	constexpr Uint8 const BannerTransparency = 0x6a;
	constexpr const long AnimationCap = 5000;

	bool animationStarted;
	bool animationComplet;
	long animationTickers;

	SDL_Rect Source = { 0,0,0,0 };
	SDL_Texture* Current = nullptr;
	SDL_Rect Destination;
	SDL_Rect uprBar = { 0,0,0,0 };
	SDL_Rect lwrBar = { 0,0,0,0 };

	bool Initialize()
	{
		const auto& logicalW = blooDot::Settings::GetLogicalArenaWidth();
		const auto& logicalH = blooDot::Settings::GetLogicalArenaHeight();
		const auto& physiclW = blooDot::Settings::GetPhysicalArenaWidth();
		const auto& physiclH = blooDot::Settings::GetPhysicalArenaHeight();
		const auto& availabW = physiclW - logicalW;
		const auto& offfsetX = availabW / 2;
		const auto& availabH = physiclH - logicalH;
		const auto& offfsetY = availabH / 2;

		Source.w = logicalW;
		Source.h = 86;

		/* why does this have three f, you easel?
		 * simple: just like The Aesthetic Weasel */
		Destination.x = offfsetX;
		Destination.y = logicalH / 2 + offfsetY - 150 / 2;
		Destination.w = logicalW;
		Destination.h = Source.h;

		uprBar.h = 14;
		lwrBar.h = 14;
		lwrBar.w = Destination.w;
		uprBar.w = Destination.w;
		uprBar.x = Destination.x;
		uprBar.y = Destination.y;
		lwrBar.x = Destination.x;
		lwrBar.y = Destination.y + Destination.h - 14;

		const auto& bannerTexture = SDL_CreateTexture(
			GameViewRenderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET,
			Destination.w,
			Destination.h
		);

		if (!bannerTexture)
		{
			const auto createError = SDL_GetError();
			ReportError("Failed to create a HUD texture", createError);
			return false;
		}

		SDL_SetTextureBlendMode(bannerTexture, SDL_BlendMode::SDL_BLENDMODE_BLEND);

		Current = bannerTexture;
		if (!Redraw(0))
		{
			return false;
		}

		return true;
	}

	bool Redraw(int ofRegion)
	{
		if (SDL_SetRenderTarget(GameViewRenderer, Current) < 0)
		{
			const auto targetError = SDL_GetError();
			ReportError("Failed to set a region banner texture as the render target", targetError);
			return false;
		}

		SDL_SetRenderDrawColor(GameViewRenderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
		SDL_RenderClear(GameViewRenderer);

		auto canvasTexture = BeginRenderDrawing(GameViewRenderer, Source.w, Source.h);
		auto const& canvas = GetDrawingSink();

		cairo_set_line_width(canvas, 3.3);
		cairo_set_source_rgb(canvas, 0.7, 1., 0);
		cairo_move_to(canvas, 0, 0);
		cairo_curve_to(
			canvas,
			300, 100, // first control point
			50, 70, // second control point
			Source.w - 5., 150); // end point

		cairo_stroke(canvas);
		std::stringstream regionName;
		regionName << ofRegion << ". " << literalregionNameScullery;
		auto regionStr = regionName.str();

		cairo_set_source_rgb(canvas, .7, .7, 1);
		cairo_select_font_face(
			canvas,
			"sans-serif",
			CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_BOLD
		);

		cairo_set_font_size(canvas, 35);
		cairo_move_to(canvas, 68, 54);
		cairo_show_text(canvas, regionStr.data());

		EndRenderDrawing(GameViewRenderer, canvasTexture);
		if (SDL_SetRenderTarget(GameViewRenderer, NULL) < 0)
		{
			const auto resetError = SDL_GetError();
			ReportError("Failed to reset the render target after rendering to region banner texture", resetError);
			return false;
		}

		_ResetAnimation();

		return true;
	}

	void _ResetAnimation()
	{
		animationComplet = false;
		animationStarted = false;
	}

	void Update()
	{
		if (animationComplet)
		{
			return;
		}

		if (!animationStarted)
		{
			animationTickers = 0;
			animationStarted = true;
		} 

		--uprBar.w;
		++lwrBar.x;

		++animationTickers;
		if (animationTickers == AnimationCap)
		{
			animationComplet = true;
		}
	}

	void Render()
	{
		SDL_SetRenderDrawColor(
			GameViewRenderer,
			BannerGray,
			BannerGray,
			BannerGray,
			BannerTransparency
		);

		if (SDL_SetRenderDrawBlendMode(GameViewRenderer, SDL_BLENDMODE_BLEND) != 0)
		{
			const auto modeError = SDL_GetError();
			ReportError("Failed to set blend mode", modeError);
			return;
		}

		SDL_RenderFillRect(GameViewRenderer, &Destination);
		SDL_RenderFillRect(GameViewRenderer, &uprBar);
		SDL_RenderFillRect(GameViewRenderer, &lwrBar);
		SDL_RenderCopy(GameViewRenderer, Current, &Source, &Destination);
	}

	void Teardown()
	{
		if(Current) 
		{
			SDL_DestroyTexture(Current);
		}
	}
}