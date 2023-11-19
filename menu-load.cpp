#include "pch.h"
#include "menu-load.h"

#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo/cairo.h>
#include "xlations.h"
#include "settings.h"
#include <regex>
#include "constants.h"
#include "sfx.h"
#include <iostream>
#include "harvard.h"
#include "geom2d.h"

constexpr int const bounceMargin = 10;
constexpr int const vignetteWidth = 250;
constexpr int const vignetteHeight = 220;
constexpr int const vignetteGap = 10;
constexpr int const screenshotReduction = 38;
constexpr int const screenshotShiftY = 16;

extern SettingsStruct Settings;
extern bool mainRunning;

using namespace blooDot::Geometry2d;
namespace blooDot::MenuLoad
{
	SDL_Event loadMenuEvent;
	bool loadMenuRunning = false;
	auto menuSelection = LoadMenuItems::LMI_CANCEL;
	auto selectedSavegame = 0;
	auto movingToSavegame = 0;
	SDL_Texture* slidingSavegames;
	int sliderTextureWidth;
	int sliderOffsetLeft = bounceMargin;
	int vignetteCount = 0;
	int lastSavegameIndex = 0;
	int slideSpeed = 0;
	int targetOffsetLeft = 0;

	bool MenuLoop(SDL_Renderer* renderer)
	{
		constexpr int const startY = 94;
		constexpr int const stride = 46;
		constexpr int backGap = stride / 2;
		constexpr int const sliderY = startY + stride + backGap;
		constexpr SDL_Rect const carouselDestRect = { 195,sliderY,vignetteWidth,vignetteHeight };
		
		_PrepareControls(renderer);
		loadMenuRunning = true;
		selectedSavegame = 0;
		movingToSavegame = 0;
		sliderOffsetLeft = static_cast<int>(selectedSavegame) * vignetteWidth + bounceMargin;

		SDL_Rect outerMenuRect{ 150,45,340,390 };
		SDL_Rect titleRect{ 0,0,0,0 };
		SDL_Rect cancelRect{ 0,0,0,0 };
		SDL_Rect hintRect{ 0,0,0,0 };
		SDL_Rect carouselSrcRect = { sliderOffsetLeft,0,vignetteWidth,vignetteHeight };

		const auto titleTexture = RenderText(
			renderer,
			&titleRect,
			FONT_KEY_ALIEN,
			26,
			literalAlienLoadMenuLabel,
			AlienTextColor
		);

		/* menu item text */
		const auto cancelTexture = RenderText(
			renderer,
			&cancelRect,
			FONT_KEY_DIALOG,
			23,
			literalMenuCancel,
			ButtonTextColor
		);

		unsigned short frame = 0L;
		while (loadMenuRunning)
		{
			while (SDL_PollEvent(&loadMenuEvent) != 0)
			{
				switch (loadMenuEvent.type)
				{
				case SDL_QUIT:
					mainRunning = false;
					loadMenuRunning = false;
					break;

				case SDL_KEYDOWN:
					switch (loadMenuEvent.key.keysym.scancode)
					{
					case SDL_SCANCODE_DOWN:
					case SDL_SCANCODE_KP_2:
					case SDL_SCANCODE_S:
					case SDL_SCANCODE_PAGEDOWN:
					case SDL_SCANCODE_END:
						if (menuSelection == LMI_SAVEGAME)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = LMI_SAVEGAME;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_UP:
					case SDL_SCANCODE_KP_8:
					case SDL_SCANCODE_W:
					case SDL_SCANCODE_PAGEUP:
					case SDL_SCANCODE_HOME:
						if (menuSelection == LMI_CANCEL)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = LMI_CANCEL;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_LEFT:
					case SDL_SCANCODE_KP_4:
					case SDL_SCANCODE_A:
						if (menuSelection == LMI_CANCEL)
						{
							menuSelection = LMI_SAVEGAME;
						}

						if (selectedSavegame == 0)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else if (movingToSavegame == selectedSavegame)
						{
							movingToSavegame = selectedSavegame - 1;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}
						else if (movingToSavegame > 0)
						{
							--movingToSavegame;
						}

						break;

					case SDL_SCANCODE_RIGHT:
					case SDL_SCANCODE_KP_6:
					case SDL_SCANCODE_D:
						if (menuSelection == LMI_CANCEL)
						{
							menuSelection = LMI_SAVEGAME;
						}

						if (selectedSavegame == lastSavegameIndex)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else if (movingToSavegame == selectedSavegame)
						{
							movingToSavegame = selectedSavegame + 1;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}
						else if (movingToSavegame < lastSavegameIndex)
						{
							++movingToSavegame;
						}

						break;

					case SDL_SCANCODE_RETURN:
					case SDL_SCANCODE_RETURN2:
					case SDL_SCANCODE_KP_ENTER:
					case SDL_SCANCODE_SPACE:
						blooDot::Sfx::Play(SoundEffect::SFX_SELCONF);
						if (menuSelection == LMI_SAVEGAME)
						{
							loadMenuRunning = false;
						}
						else if (menuSelection == LMI_CANCEL)
						{
							loadMenuRunning = false;
						}

						break;

					case SDL_SCANCODE_ESCAPE:
						loadMenuRunning = false;
						break;
					}

					break;
				}
			}

			if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = IMG_GetError();
				ReportError("Failed to clear the load menu screen", clearError);
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
			SDL_RenderFillRect(renderer, &outerMenuRect);
			SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
			if (SDL_RenderDrawRect(renderer, &outerMenuRect) < 0)
			{
				const auto drawRectError = SDL_GetError();
				ReportError("Failed to draw load menu panel border", drawRectError);
				loadMenuRunning = false;
			};

			DrawLabel(renderer, 286, 51, titleTexture, &titleRect);

			const auto drawingTexture = BeginRenderDrawing(renderer, 640, 480);
			if (drawingTexture) [[likely]]
			{
				auto const& drawingSink = GetDrawingSink();
				LoadMenuItems itemToDraw = LMI_CANCEL;
				for (auto y = startY; y < 200; y += stride)
				{
					DrawButton(
						drawingSink,
						195,
						y,
						vignetteWidth,
						itemToDraw > LMI_CANCEL ? vignetteHeight : 42,
						itemToDraw == menuSelection ? CH_MAINMENU : CH_NONE
					);

					if (itemToDraw == menuSelection)
					{
						if (menuSelection == LMI_CANCEL)
						{
							DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
							DrawChevron(drawingSink, 195 + vignetteWidth + 7, y + 21, true, frame);
						}
						else
						{
							DrawChevron(drawingSink, 195 - 9, y + 105, true, frame);
							DrawChevron(drawingSink, 195 + vignetteWidth + 9, y + 105, false, frame);
						}
					}

					if (itemToDraw == LMI_CANCEL)
					{
						y += backGap;
					}

					itemToDraw = static_cast<LoadMenuItems>(itemToDraw + 1);
				}

				EndRenderDrawing(renderer, drawingTexture, nullptr);
				DrawLabel(renderer, 235, startY + 0 * stride + 0 * backGap, cancelTexture, &cancelRect);

				/* render the carousel choice (and the sliding animation) */
				_AnimateCarousel();
				carouselSrcRect.x = sliderOffsetLeft;
				SDL_RenderCopy(renderer, slidingSavegames, &carouselSrcRect, &carouselDestRect);
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		_Teardown();
		titleTexture&& [titleTexture] { SDL_DestroyTexture(titleTexture); return false; }();
		cancelTexture&& [cancelTexture] { SDL_DestroyTexture(cancelTexture); return false; }();

		return loadMenuRunning;
	}

	void _AnimateCarousel()
	{
		if (movingToSavegame == selectedSavegame)
		{
			slideSpeed = 0;
			return;
		}

		if (slideSpeed == 0)
		{
			targetOffsetLeft = bounceMargin + movingToSavegame * vignetteWidth;
			slideSpeed = 40;
		}

		auto direction = targetOffsetLeft < sliderOffsetLeft ? -1 : 1;
		sliderOffsetLeft += direction * slideSpeed;
		if (direction == 1)
		{
			if (sliderOffsetLeft >= targetOffsetLeft)
			{
				sliderOffsetLeft = targetOffsetLeft;
				selectedSavegame = movingToSavegame;
			}
		}
		else
		{
			if (sliderOffsetLeft <= targetOffsetLeft)
			{
				sliderOffsetLeft = targetOffsetLeft;
				selectedSavegame = movingToSavegame;
			}
		}
	}

	void _PrepareScreenshotRect(SDL_Rect* rect, int vignetteIndex)
	{
		*rect = { bounceMargin + 30 + vignetteIndex * vignetteWidth, 102, vignetteWidth - 60, 64 };
	}

	void _PrepareControls(SDL_Renderer* renderer)
	{
		auto screenshotSrc = SDL_Rect { 0,0,200,120 };
		const auto& numberOfSavegames = SetBitCount(::Settings.OccupiedSavegameSlots);
		vignetteCount = std::max(1, numberOfSavegames);
		sliderTextureWidth = vignetteCount * vignetteWidth + (vignetteCount - 1) * vignetteGap + 2 * bounceMargin;
		slidingSavegames = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET,
			sliderTextureWidth,
			vignetteHeight
		);

		if (slidingSavegames)
		{
			if (SDL_SetTextureBlendMode(slidingSavegames, SDL_BLENDMODE_BLEND) < 0)
			{
				const auto carouselBlendmodeError = SDL_GetError();
				ReportError("Could not set blend mode of sliding texture", carouselBlendmodeError);
				SDL_DestroyTexture(slidingSavegames);
				slidingSavegames = NULL;

				return;
			}

			if (SDL_SetRenderTarget(renderer, slidingSavegames) < 0)
			{
				const auto targetError = SDL_GetError();
				ReportError("Could not set sliding texture as the render target", targetError);
				return;
			}

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			auto bitIndex = 0;
			auto ordinal = 0;
			for (auto i = 0; i < numberOfSavegames; ++i)
			{
				while (!(::Settings.OccupiedSavegameSlots & (1 << bitIndex)))
				{
					++bitIndex;
				}

				const auto savegameIndex = bitIndex + 1;
				const auto& savegameData = blooDot::Savegame::LoadInfoShallow(
					renderer,
					savegameIndex
				);

				std::stringstream formatter;
				formatter << literalSavegame << " #" << savegameData.Header.SavegameIndex;
				std::string formatted;
				formatted = formatter.str();
				_VignetteLabel(renderer, FONT_KEY_DIALOG, 28, ordinal, 30, formatted.c_str());

				formatter.str(std::string());
				formatter.clear();
				if (savegameData.Savepoints.size() == 0)
				{
					formatter << literalSavegameBlank;
				}
				else if (savegameData.Savepoints.size() == 1)
				{
					formatter << literalSavepointSingular;
				}
				else
				{
					std::stringstream savepointCount;
					savepointCount << savegameData.Savepoints.size();
					formatter << std::regex_replace(literalSavepointPlural, std::regex("\\$n"), savepointCount.str());
				}

				formatted = formatter.str();
				_VignetteLabel(renderer, FONT_KEY_DIALOG, 13, ordinal, 70, formatted.c_str());

				formatter.str(std::string());
				formatter.clear();
				formatter << literalCreated << " ";
				blooDot::Datetime::Format(&savegameData.Header.Created, &formatter);

				formatted = formatter.str();
				_VignetteLabel(renderer, FONT_KEY_DIALOG, 13, ordinal, 190, formatted.c_str());
				if (savegameData.MostRecentScreenshot)
				{
					SDL_Rect centerRect;
					SDL_Rect destRect;

					_PrepareScreenshotRect(&destRect, ordinal);
					CenterRectInRect(&destRect, &screenshotSrc, &centerRect);
					centerRect.w -= screenshotReduction;
					centerRect.h -= screenshotReduction;
					centerRect.x += screenshotReduction / 2;
					centerRect.y += screenshotShiftY;
					SDL_RenderCopy(
						renderer,
						savegameData.MostRecentScreenshot,
						&screenshotSrc,
						&centerRect
					);
				}

				++ordinal;
				++bitIndex;
			}

			if (SDL_SetRenderTarget(renderer, NULL) < 0)
			{
				const auto restoreError = SDL_GetError();
				ReportError("Could not restore render target after rendering to sliding texture", restoreError);
				return;
			}

			lastSavegameIndex = vignetteCount - 1;
			return;
		}

		const auto newCarouselError = SDL_GetError();
		ReportError("Could not allocate sliding texture", newCarouselError);
	}

	void _VignetteLabel(SDL_Renderer* renderer, int font, int size, int vignetteIndex, int y, const char* text)
	{
		SDL_Rect rectLabel = { 0,0,0,0 };
		const auto textureLabel = RenderText(
			renderer,
			&rectLabel,
			font,
			size,
			text,
			{ 250, 230, 230, 245 }
		);

		CenterLabel(
			renderer,
			bounceMargin + vignetteIndex * vignetteWidth + vignetteWidth / 2,
			y,
			textureLabel,
			&rectLabel
		);

		SDL_DestroyTexture(textureLabel);
	}

	void _Teardown()
	{
		DestroyTexture(&slidingSavegames);
	}
}