#include "gpuchan.h"

#include "dialog-controls.h"
#include "drawing.h"
#include "menu-common.h"
#include "scripture.h"
#include "../shared-constants.h"
#include "../../res/xlations.h"
#include "../../res/chunk-constants.h"
#include "SDL3/SDL_pixels.h"
#include "../../main.h"
#include "../../src/util/resutil.h"

namespace blooDot::Ui::GpuChan {
	constexpr SDL_Color titleTextColor = { 11,8,8,222 };
	constexpr SDL_Color bubbleTextColor = { 12, 8, 8, 255 };

	MenuCommon::MenuDialogInteraction menuState;
	SDL_Texture* gpuChan = nullptr;
	SDL_FRect chanDims = {};
	SDL_Event gpuChanEvent;

	bool GpuChanLoop(SDL_Renderer* renderer, const char* message, const char* title, const std::string& bubble)
	{
		menuState.leaveDialog = false;
		menuState.dialogResult = Constants::DMR_CANCEL;

		EnterInternal(renderer);
		constexpr SDL_FRect screen = { 0,0,Constants::GodsPreferredWidth,Constants::GodsPreferredHight };
		const SDL_FRect center = { screen.w - chanDims.w, screen.h - chanDims.h, chanDims.w, chanDims.h};
		SDL_FRect messageRect{ 0,0,0,0 };
		SDL_FRect titleRect{ 0,0,0,0 };
		SDL_FRect okRect{ 0,0,0,0 };
		SDL_FRect bubbleRect{ 0,0,0,0 };

		const auto messageTexture = Scripture::RenderText(
			renderer,
			&messageRect,
			Scripture::FONT_KEY_DIALOG,
			17,
			message,
			titleTextColor
		);

		const auto titleTexture = Scripture::RenderText(
			renderer,
			&titleRect,
			Scripture::FONT_KEY_DIALOG,
			25,
			title,
			titleTextColor
		);

		const auto okTexture = Scripture::RenderText(
			renderer,
			&okRect,
			Scripture::FONT_KEY_DIALOG_FAT,
			23,
			literalMenuOk,
			Constants::ButtonTextColor
		);

		const auto bubbleTexture = Scripture::RenderText(
			renderer,
			&bubbleRect,
			Scripture::FONT_KEY_FANCY,
			23,
			bubble.c_str(),
			bubbleTextColor
		);

		unsigned short frame = 0;
		while (!menuState.leaveDialog)
		{
			while (SDL_PollEvent(&gpuChanEvent) != 0)
			{
				switch (gpuChanEvent.type)
				{
					case SDL_EVENT_QUIT:
						menuState.dialogResult = Constants::DMR_QUIT_MAIN;
						menuState.leaveDialog = true;
						break;

					case SDL_EVENT_KEY_DOWN:
						switch (gpuChanEvent.key.scancode)
						{
						case SDL_SCANCODE_RETURN:
						case SDL_SCANCODE_RETURN2:
						case SDL_SCANCODE_KP_ENTER:
						case SDL_SCANCODE_SPACE:
						case SDL_SCANCODE_ESCAPE:
							menuState.leaveDialog = true;
							break;

						default:
							break;
						}

					default:
						break;
				}
			}

			if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = SDL_GetError();

				ReportError("Failed to wipe Gpu-chan", clearError);
			}

			if (const auto drawingTexture = Drawing::BeginRenderDrawing(renderer, Constants::GodsPreferredWidth, Constants::GodsPreferredHight))
			{
				auto const& drawingSink = Drawing::GetDrawingSink();

				DialogControls::DrawButton(
					drawingSink,
					28,
					419,
					120,
					42,
					//isCreatorMode ? Constants::CH_CREATORMODE : Constants::CH_MAINMENU
					Constants::CH_MAINMENU
				);

				DialogControls::DrawChevron(drawingSink, 28 - 7, 419 + 21, false, frame);
				DialogControls::DrawChevron(drawingSink, 28 + 120 + 7, 419 + 21, true, frame);
				Drawing::EndRenderDrawing(renderer, drawingTexture, nullptr);
			}

			SDL_RenderTexture(renderer, gpuChan, &chanDims, &center);

			DialogControls::DrawLabel(renderer, 28, 12, messageTexture, &messageRect);
			DialogControls::CenterLabel(renderer, 88, 438, okTexture, &okRect);
			DialogControls::DrawLabel(renderer, 28, 116, titleTexture, &titleRect);
			DialogControls::DrawLabel(renderer, 271, 337, bubbleTexture, &bubbleRect);

			SDL_RenderPresent(renderer);
			SDL_Delay(12);
			++frame;
		}

		PullOutInternal();
		messageTexture && [messageTexture] { SDL_DestroyTexture(messageTexture); return false; }();
		titleTexture && [titleTexture] { SDL_DestroyTexture(titleTexture); return false; }();
		okTexture && [okTexture] { SDL_DestroyTexture(okTexture); return false; }();
		bubbleTexture && [bubbleTexture] { SDL_DestroyTexture(bubbleTexture); return false; }();

		return menuState.dialogResult != Constants::DMR_QUIT_MAIN;
	}

	void EnterInternal(SDL_Renderer* renderer)
	{
		gpuChan = Res::LoadPicture(renderer, CHUNK_KEY_GPU_CHAN, &chanDims);
		if (!gpuChan)
		{
			menuState.leaveDialog = true;
		}
	}

	void PullOutInternal()
	{
		if (gpuChan)
		{
			SDL_DestroyTexture(gpuChan);
		}
	}
}
