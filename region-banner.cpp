#include "pch.h"
#include "region-banner.h"
#include "settings.h"
#include "drawing.h"
#include "xlations.h"

extern SDL_Renderer* GameViewRenderer;
extern SettingsStruct Settings;

namespace blooDot::RegionBanner
{
	constexpr Uint8 const BannerReddish = 0x85;
	constexpr Uint8 const BannerGray = 0x80;
	constexpr Uint8 const BannerTransparency = 0x6a;
	constexpr const long AnimationCap = 5000;
	constexpr const int nearIndent = 68;

	/// <summary>
	/// These banners' animations go like that:
	/// we have the upper bar coming in from the far,
	/// the lower bar coming in from the near,
	/// and then the lower bar indents from the near to
	/// until where the text is on the x,
	/// which the upper bar mirrors from the far.
	/// </summary>
	bool animationStarted;
	bool animationComplet;
	long animationTickers;
	int animationTraveled;
	int animationDistance1;
	int animationDistance2;
	int animationPhase;
	int animationShved;

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

		animationDistance1 = lwrBar.w;
		animationDistance2 = nearIndent;
		lwrBar.w = 0;
		uprBar.x = Destination.x + Destination.w;
		uprBar.w = 0;

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
			ReportError("Failed to create a region banner texture", createError);
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
		cairo_set_source_rgb(canvas, 0.7, 1., ofRegion);
		cairo_move_to(canvas, 0, 0);
		cairo_curve_to(
			canvas,
			300, 100, // first control point
			50, 70, // second control point
			Source.w - 5., 150); // end point

		cairo_stroke(canvas);
		std::stringstream regionName;
		regionName << literalregionNameOceanOfImmeasurableDisappointment;
		auto regionStr = regionName.str();

		cairo_set_source_rgb(canvas, .7, .7, 1);
		cairo_select_font_face(
			canvas,
			"sans-serif",
			CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_BOLD
		);

		cairo_move_to(canvas, nearIndent, 54);
		cairo_set_antialias(canvas, CAIRO_ANTIALIAS_SUBPIXEL);
		cairo_set_font_size(canvas, 35);
		cairo_font_options_t* fo = cairo_font_options_create();
		cairo_get_font_options(canvas, fo);
		cairo_font_options_set_antialias(fo, CAIRO_ANTIALIAS_SUBPIXEL);
		cairo_set_font_options(canvas, fo);
		cairo_show_text(canvas, regionStr.data());

		EndRenderDrawing(GameViewRenderer, canvasTexture, nullptr);
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
		animationTraveled = 0;
		animationPhase = 1;
		animationShved = 0;
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
			animationShved = 10;
		} 

		if (animationPhase == 2)
		{
			lwrBar.x += animationShved;
			lwrBar.w -= animationShved;
			uprBar.w -= animationShved;
			animationTraveled += animationShved;

			if (animationTraveled >= animationDistance1 + animationDistance2)
			{
				animationPhase = 0;
				animationComplet = true;
			}
		}

		if (animationPhase == 1)
		{
			uprBar.x -= animationShved;
			uprBar.w += animationShved;
			lwrBar.w += animationShved;
			animationTraveled += animationShved;

			if (animationTraveled >= animationDistance1)
			{
				animationPhase = 2;
			}
		}

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
			BannerReddish,
			BannerGray,
			BannerGray,
			BannerTransparency
		);

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