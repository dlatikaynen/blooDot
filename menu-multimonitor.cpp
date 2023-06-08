#include "pch.h"
#include "menu-multimonitor.h"

#include "scripture.h"
#include "drawing.h"
#include "dialogcontrols.h"
#include <cairo.h>
#include "xlations.h"
#include "settings.h"
#include "constants.h"
#include <iostream>
#include <regex>

constexpr int const bounceMargin = 10;
constexpr int const vignetteWidth = 250;
constexpr int const vignetteGap = 10;

extern SettingsStruct Settings;
extern bool mainRunning;

namespace blooDot::MultiMonitorMenuScreen
{
	constexpr int const vignetteHeight = 234;
	constexpr const SDL_Color labelColor = { 250, 230, 230, 245 };
	constexpr const SDL_Color accentColor = { 7, 29, 215, 156 };

	SDL_Event monitorChoiceMenuEvent;
	bool multiMonitorMenuRunning = false;
	bool confirmed = false;
	auto menuSelection = MultiMonitorMenuItems::MMMI_DISPLAY;
	auto selectedDisplay = 0;
	auto movingToDisplay = 0;
	int vignetteCount = 0;
	std::vector<DisplayRepresentation> representations;
	SDL_Texture* slidingMonitors;
	int sliderTextureWidth;
	int sliderOffsetLeft = bounceMargin;
	int slideSpeed = 0;
	int targetOffsetLeft = 0;

	bool MultiMonitorMenuLoop(SDL_Renderer* renderer)
	{
		constexpr int const startY = 94;
		constexpr int const stride = 46;
		constexpr int const labelInsetX = 45;
		constexpr int backGap = stride / 2;
		constexpr int const sliderY = startY + stride + backGap;

		constexpr SDL_Rect outerMenuRect{ 150,45,340,390 };
		constexpr SDL_Rect const carouselDestRect = { outerMenuRect.x + labelInsetX,sliderY,vignetteWidth,vignetteHeight };

		_PrepareControls(renderer);
		multiMonitorMenuRunning = true;
		sliderOffsetLeft = selectedDisplay * vignetteWidth + bounceMargin;

		SDL_Rect titleRect{ 0,0,0,0 };
		SDL_Rect cancelRect{ 0,0,0,0 };
		SDL_Rect hintRect{ 0,0,0,0 };
		SDL_Rect carouselSrcRect = { sliderOffsetLeft,0,vignetteWidth,vignetteHeight };

		const auto titleTexture = RenderText(
			renderer,
			&titleRect,
			FONT_KEY_ALIEN,
			26,
			literalAlienMultiMonitorMenuLabel,
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
		while (multiMonitorMenuRunning)
		{
			while (SDL_PollEvent(&monitorChoiceMenuEvent) != 0)
			{
				switch (monitorChoiceMenuEvent.type)
				{
				case SDL_QUIT:
					mainRunning = false;
					multiMonitorMenuRunning = false;
					break;

				case SDL_KEYDOWN:
					switch (monitorChoiceMenuEvent.key.keysym.scancode)
					{
					case SDL_SCANCODE_DOWN:
					case SDL_SCANCODE_KP_2:
					case SDL_SCANCODE_S:
					case SDL_SCANCODE_PAGEDOWN:
					case SDL_SCANCODE_END:
						if (menuSelection == MMMI_DISPLAY)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = MMMI_DISPLAY;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_UP:
					case SDL_SCANCODE_KP_8:
					case SDL_SCANCODE_W:
					case SDL_SCANCODE_PAGEUP:
					case SDL_SCANCODE_HOME:
						if (menuSelection == MMMI_CANCEL)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else
						{
							menuSelection = MMMI_CANCEL;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}

						break;

					case SDL_SCANCODE_LEFT:
					case SDL_SCANCODE_KP_4:
					case SDL_SCANCODE_A:
						if (menuSelection == MMMI_CANCEL)
						{
							menuSelection = MMMI_DISPLAY;
						}

						if (selectedDisplay == 0)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else if (movingToDisplay == selectedDisplay)
						{
							movingToDisplay = selectedDisplay - 1;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}
						else if (movingToDisplay != 0)
						{
							--movingToDisplay;
						}

						break;

					case SDL_SCANCODE_RIGHT:
					case SDL_SCANCODE_KP_6:
					case SDL_SCANCODE_D:
						if (menuSelection == MMMI_CANCEL)
						{
							menuSelection = MMMI_DISPLAY;
						}

						if (selectedDisplay == vignetteCount - 1)
						{
							blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
						}
						else if (movingToDisplay == selectedDisplay)
						{
							movingToDisplay = selectedDisplay + 1;
							blooDot::Sfx::Play(SoundEffect::SFX_SELCHG);
						}
						else if (movingToDisplay != vignetteCount - 1)
						{
							++movingToDisplay;
						}

						break;

					case SDL_SCANCODE_RETURN:
					case SDL_SCANCODE_RETURN2:
					case SDL_SCANCODE_KP_ENTER:
					case SDL_SCANCODE_SPACE:
						if (menuSelection == MMMI_DISPLAY)
						{
							if (movingToDisplay >= 0 && movingToDisplay < representations.size()) 
							{
								const auto& representation = representations.at(movingToDisplay);
								::Settings.FullscreenDisplayIndex = static_cast<unsigned char>(representation.displayIndex);
								confirmed = true;
								multiMonitorMenuRunning = false;
							}
							else 
							{
								blooDot::Sfx::Play(SoundEffect::SFX_ASTERISK);
								break;
							}
						}
						else if (menuSelection == MMMI_CANCEL)
						{
							multiMonitorMenuRunning = false;
						}

						blooDot::Sfx::Play(SoundEffect::SFX_SELCONF);
						break;

					case SDL_SCANCODE_ESCAPE:
						multiMonitorMenuRunning = false;
						break;
					}

					break;
				}
			}

			if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = IMG_GetError();
				ReportError("Failed to clear the screen settings menu screen", clearError);
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
			SDL_RenderFillRect(renderer, &outerMenuRect);
			SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
			if (SDL_RenderDrawRect(renderer, &outerMenuRect) < 0)
			{
				const auto drawRectError = SDL_GetError();
				ReportError("Failed to draw screen settings menu panel border", drawRectError);
				multiMonitorMenuRunning = false;
			};

			DrawLabel(renderer, 286, 51, titleTexture, &titleRect);

			const auto drawingTexture = BeginRenderDrawing(renderer, 640, 480);
			if (drawingTexture) [[likely]]
			{
				auto const& drawingSink = GetDrawingSink();

				MultiMonitorMenuItems itemToDraw = MMMI_CANCEL;
				constexpr int const buttonLeft = outerMenuRect.x + labelInsetX;
				for (auto y = startY; y < 200; y += stride)
				{
					DrawButton(
						drawingSink,
						buttonLeft,
						y,
						vignetteWidth,
						itemToDraw > MMMI_CANCEL ? vignetteHeight : 42,
						itemToDraw == menuSelection
					);

					if (itemToDraw == menuSelection)
					{
						if (menuSelection == MMMI_CANCEL)
						{
							DrawChevron(drawingSink, buttonLeft - 7, y + 21, false, frame);
							DrawChevron(drawingSink, buttonLeft + vignetteWidth + 7, y + 21, true, frame);
						}
						else
						{
							if (movingToDisplay > 0 && selectedDisplay > 0)
							{
								DrawChevron(drawingSink, buttonLeft - 9, y + 105, true, frame);
							}

							if (movingToDisplay < vignetteCount - 1 && selectedDisplay < vignetteCount - 1)
							{
								DrawChevron(drawingSink, buttonLeft + vignetteWidth + 9, y + 105, false, frame);
							}
						}
					}

					if (itemToDraw == MMMI_CANCEL)
					{
						y += backGap;
					}

					itemToDraw = static_cast<MultiMonitorMenuItems>(itemToDraw + 1);
				}

				EndRenderDrawing(renderer, drawingTexture, nullptr);
				DrawLabel(renderer, 235, startY + 0 * stride + 0 * backGap, cancelTexture, &cancelRect);

				/* render the carousel choice (and the sliding animation) */
				_AnimateCarousel();
				carouselSrcRect.x = sliderOffsetLeft;
				SDL_RenderCopy(renderer, slidingMonitors, &carouselSrcRect, &carouselDestRect);
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		_Teardown();
		titleTexture&& [titleTexture] { SDL_DestroyTexture(titleTexture); return false; }();
		cancelTexture&& [cancelTexture] { SDL_DestroyTexture(cancelTexture); return false; }();

		return confirmed;
	}

	void _AnimateCarousel()
	{
		if (movingToDisplay == selectedDisplay)
		{
			slideSpeed = 0;
			return;
		}

		if (slideSpeed == 0)
		{
			targetOffsetLeft = bounceMargin + movingToDisplay * vignetteWidth;
			slideSpeed = 40;
		}

		auto direction = targetOffsetLeft < sliderOffsetLeft ? -1 : 1;
		sliderOffsetLeft += direction * slideSpeed;
		if (direction == 1)
		{
			if (sliderOffsetLeft >= targetOffsetLeft)
			{
				sliderOffsetLeft = targetOffsetLeft;
				selectedDisplay = movingToDisplay;
			}
		}
		else
		{
			if (sliderOffsetLeft <= targetOffsetLeft)
			{
				sliderOffsetLeft = targetOffsetLeft;
				selectedDisplay = movingToDisplay;
			}
		}
	}

	void _PrepareControls(SDL_Renderer* renderer)
	{
		representations.clear();
		vignetteCount = SDL_GetNumVideoDisplays();
		if (vignetteCount < 2)
		{
			std::cerr << "Multimonitor selection called althoug there are less than two displays, aborting";
			return;
		}

		sliderTextureWidth = vignetteCount * vignetteWidth + (vignetteCount - 1) * vignetteGap + 2 * bounceMargin;
		slidingMonitors = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET,
			sliderTextureWidth,
			vignetteHeight
		);

		if (slidingMonitors)
		{
			/* compute the display (monitor) arrangement
			 * using a downscaled rendition of their screen rects.
			 * we also use the horizontal offset ordering to represent
			 * an accurate left-to-right listing of the monitors,
			 * because everything else is annoying and unsophisticated */
			SDL_Rect maximums = { 0 };
			bool doDrawArrangement = true;
			bool anyMonitors = false;
			for (int i = 0; i < vignetteCount; ++i)
			{
				DisplayRepresentation bounds = { 0 };
				bounds.displayIndex = i;
				if (SDL_GetDisplayBounds(i, &bounds.rect) < 0)
				{
					const auto boundsError = SDL_GetError();
					ReportError("Failed to obtain display bounds", boundsError);
					doDrawArrangement = false;
					anyMonitors = false;
				}
				else
				{
					/* there is no need to query for orientation separately,
					 * as the representations' aspects will retain the orientations
					 * just fine */
					representations.push_back(bounds);
					anyMonitors = true;
				}

				maximums.x = std::min(bounds.rect.x, maximums.x);
				maximums.y = std::min(bounds.rect.y, maximums.y);
				maximums.w = std::max(bounds.rect.x + bounds.rect.w, maximums.w);
				maximums.h = std::max(bounds.rect.y + bounds.rect.h, maximums.h);
			}

			std::sort(representations.begin(), representations.end(), [](DisplayRepresentation& a, DisplayRepresentation& b)
			{
				return a.rect.x < b.rect.x; 
			});

			if (anyMonitors && (representations.size() != vignetteCount || maximums.w == 0 || maximums.h == 0))
			{
				/* we don't draw representations of the arrangement
				 * if one of the constituents could not be fetched */
				anyMonitors = false;
			}
			else
			{
				/* move them so they start at zero, which 
				 * is where I am on the scale of caring */
				if (maximums.x < 0)
				{
					auto shiftX = -maximums.x;
					for (auto& representation : representations)
					{
						representation.rect.x += shiftX;
					}

					maximums.w -= maximums.x;
					maximums.x = 0;
				}

				if (maximums.y < 0)
				{
					auto shiftY = -maximums.y;
					for (auto& representation : representations)
					{
						representation.rect.y += shiftY;
					}

					maximums.h -= maximums.y;
					maximums.y = 0;
				}
			}

			if (SDL_SetTextureBlendMode(slidingMonitors, SDL_BLENDMODE_BLEND) < 0)
			{
				const auto carouselBlendmodeError = SDL_GetError();
				ReportError("Could not set blend mode of sliding texture", carouselBlendmodeError);
				SDL_DestroyTexture(slidingMonitors);
				slidingMonitors = NULL;

				return;
			}

			if (SDL_SetRenderTarget(renderer, slidingMonitors) < 0)
			{
				const auto targetError = SDL_GetError();
				ReportError("Could not set sliding texture as the render target", targetError);
				return;
			}

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			for (int i = 0; i < vignetteCount; ++i)
			{
				auto& displayDescriptor = representations.at(i);
				int displayIndex = displayDescriptor.displayIndex;

				/* 1. show the ordinal of the display */
				std::stringstream displayNumber;
				displayNumber << (i + 1);
				std::string displayNumberTemplate;
				displayNumberTemplate.assign(literalMultiMonitorMenuDisplayLabel);
				auto displayNumberLabel = std::regex_replace(displayNumberTemplate, std::regex("\\$n"), displayNumber.str());
				_VignetteLabel(renderer, FONT_KEY_DIALOG, 28, i, 30, displayNumberLabel.c_str());

				/* 2. show resolution and refresh rate */
				SDL_DisplayMode mode;
				if (SDL_GetDesktopDisplayMode(displayIndex, &mode) < 0)
				{
					const auto modeError = SDL_GetError();
					ReportError("Failed to query display mode", modeError);
				}
				else 
				{
					std::stringstream displayMode;
					displayMode << mode.w << " x " << mode.h << " @ " << mode.refresh_rate << " Hz";
					auto displayModeLabel = displayMode.str();
					_VignetteLabel(renderer, FONT_KEY_DIALOG_FAT, 13, i, 70, displayModeLabel.c_str());
				}

				/* 3. show the name of the display model */
				const auto& displayName = SDL_GetDisplayName(displayIndex);
				_VignetteLabel(renderer, FONT_KEY_DIALOG, 23, i, 204, displayName);

				/* 4. draw a scaled representation of the display
				 * arrangement, highlighting the current display */
				if (anyMonitors)
				{
					SDL_Rect arrangementBounds;
					_PrepareRepresentationRect(&arrangementBounds, i);

					/* the scaled arrangement is a square. therefore,
					 * we must determine the orientation of the true
					 * arrangement so we can letterbox it properly */
					float scaleFactor = 0;
					if (maximums.h > maximums.w)
					{
						scaleFactor = static_cast<float>(arrangementBounds.w) / static_cast<float>(maximums.w);
					}
					else
					{
						scaleFactor = static_cast<float>(arrangementBounds.h) / static_cast<float>(maximums.h);
					}

					int runningIndex = 0;
					int offsetX = static_cast<int>((static_cast<float>(arrangementBounds.w) - static_cast<float>(maximums.w) * scaleFactor) / 2.);
					int offsetY = static_cast<int>((static_cast<float>(arrangementBounds.h) - static_cast<float>(maximums.h) * scaleFactor) / 2.);
					for (const auto& representation : representations)
					{
						SDL_Rect scaledRepresentation;
						scaledRepresentation.x = static_cast<int>(static_cast<float>(representation.rect.x) * scaleFactor) + offsetX;
						scaledRepresentation.y = static_cast<int>(static_cast<float>(representation.rect.y) * scaleFactor) + offsetY;
						scaledRepresentation.w = static_cast<int>(static_cast<float>(representation.rect.w) * scaleFactor);
						scaledRepresentation.h = static_cast<int>(static_cast<float>(representation.rect.h) * scaleFactor);
						_DisplayRepresentation(renderer, &scaledRepresentation, i, i == runningIndex++);
					}
				}
			}

			if (SDL_SetRenderTarget(renderer, NULL) < 0)
			{
				const auto restoreError = SDL_GetError();
				ReportError("Could not restore render target after rendering to sliding texture", restoreError);
				return;
			}

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
			labelColor
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

	void _PrepareRepresentationRect(SDL_Rect* rect, int vignetteIndex)
	{
		*rect = {
			bounceMargin + 30 + vignetteIndex * vignetteWidth,
			102,
			vignetteWidth - 60,
			74
		};
	}

	void _DisplayRepresentation(SDL_Renderer* renderer, SDL_Rect* dimension, int vignetteIndex, bool isCurrent)
	{
		SDL_Rect finalRect = {
			bounceMargin + 30 + vignetteIndex * vignetteWidth + dimension->x,
			102 + dimension->y,
			dimension->w,
			dimension->h
		};

#ifndef NDEBUG
		SDL_Rect arra;
		_PrepareRepresentationRect(&arra, vignetteIndex);
		SDL_SetRenderDrawColor(renderer, 255, 196, 4, 120);
		SDL_RenderDrawRect(renderer, &arra);
#endif
		if (isCurrent)
		{
			SDL_SetRenderDrawColor(renderer, accentColor.r, accentColor.g, accentColor.b, accentColor.a);
			SDL_RenderFillRect(renderer, &finalRect);
		}

		SDL_SetRenderDrawColor(renderer, labelColor.r, labelColor.g, labelColor.b, labelColor.a);		
		SDL_RenderDrawRect(renderer, &finalRect);
	}

	void _Teardown()
	{
		slidingMonitors&& [] { SDL_DestroyTexture(slidingMonitors); return false; }();
	}
}