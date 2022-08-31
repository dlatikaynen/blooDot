#include "pch.h"
#include "gpuchan.h"
#include "dialogcontrols.h"

SDL_Texture* gpuChan = NULL;
SDL_Rect chanDims = { 0 };
bool chanRunning = false;
SDL_Event gpuChanEvent;

bool GpuChanLoop(SDL_Renderer* renderer, const char* message, const char* title, std::string bubble)
{
	chanRunning = true;
	_Enter(renderer);
	SDL_Rect screen = { 0,0,640,480 };
	SDL_Rect center = { screen.w - chanDims.w, screen.h - chanDims.h, chanDims.w, chanDims.h};
	SDL_Rect messageRect{ 0,0,0,0 };
	SDL_Rect titleRect{ 0,0,0,0 };
	SDL_Rect okRect{ 0,0,0,0 };
	SDL_Rect bubbleRect{ 0,0,0,0 };

	const auto messageTexture = RenderText(
		renderer,
		&messageRect,
		FONT_KEY_TITLE,
		13,
		message,
		{ 11,8,8,222 }
	);

	const auto titleTexture = RenderText(
		renderer,
		&titleRect,
		FONT_KEY_TITLE,
		25,
		title,
		{ 11,8,8,222 }
	);

	const auto okTexture = RenderText(
		renderer,
		&okRect,
		FONT_KEY_DIALOG,
		23,
		literalMenuOk,
		{ 250, 230, 230, 245 }
	);

	/* explanation of why the settings window itself does not change */
	const auto bubbleTexture = RenderText(
		renderer,
		&bubbleRect,
		FONT_KEY_DIALOG,
		23,
		bubble.c_str(),
		{ 12, 8, 8, 255 }
	);

	unsigned short frame = 0;
	while (chanRunning)
	{
		while (SDL_PollEvent(&gpuChanEvent) != 0)
		{
		}

		if (SDL_RenderClear(renderer) < 0)
		{
			const auto clearError = IMG_GetError();
			ReportError("Failed to wipe Gpu-chan", clearError);
		}

		const auto drawingTexture = BeginRenderDrawing(renderer, 640, 480);
		if (drawingTexture) [[likely]]
		{
			auto const& drawingSink = GetDrawingSink();
			DrawButton(
				drawingSink,
				28,
				419,
				120,
				42,
				true
			);

			DrawChevron(drawingSink, 28 - 7, 419 + 21, false, frame);
			DrawChevron(drawingSink, 28 + 120 + 7, 419 + 21, true, frame);
			EndRenderDrawing(renderer, drawingTexture);
		}

		SDL_RenderCopy(renderer, gpuChan, &chanDims, &center);

		DrawLabel(renderer, 28, 15, messageTexture, &messageRect);
		CenterLabel(renderer, 88, 438, okTexture, &okRect);
		DrawLabel(renderer, 28, 119, titleTexture, &titleRect);
		DrawLabel(renderer, 288, 332, bubbleTexture, &bubbleRect);

		SDL_RenderPresent(renderer);
		SDL_Delay(12);
		++frame;
	}

	_PullOut();
	messageTexture&& [messageTexture] { SDL_DestroyTexture(messageTexture); return false; }();
	titleTexture&& [titleTexture] { SDL_DestroyTexture(titleTexture); return false; }();
	okTexture&& [okTexture] { SDL_DestroyTexture(okTexture); return false; }();
	bubbleTexture&& [bubbleTexture] { SDL_DestroyTexture(bubbleTexture); return false; }();

	return chanRunning;
}

void _Enter(SDL_Renderer* renderer)
{
	SDL_RWops* chanStream;
	const auto chanMem = Retrieve(CHUNK_KEY_GPU_CHAN, &chanStream);
	if (!chanMem)
	{
		chanRunning = false;
		return;
	}

	const auto chanPicture = IMG_LoadPNG_RW(chanStream);
	chanStream->close(chanStream);
	SDL_free(chanMem);
	if (!chanPicture)
	{
		const auto chanLoadError = IMG_GetError();
		ReportError("Failed to load GPU-chan", chanLoadError);
		chanRunning = false;
		return;
	}

	gpuChan = SDL_CreateTextureFromSurface(renderer, chanPicture);
	if (!gpuChan)
	{
		const auto textureError = SDL_GetError();
		SDL_free(chanPicture);
		ReportError("Failed to create gpu-chan no texture", textureError);
		chanRunning = false;
		return;
	}

	chanDims.w = chanPicture->w;
	chanDims.h = chanPicture->h;
	SDL_free(chanPicture);
}

void _PullOut()
{
	if (gpuChan)
	{
		SDL_DestroyTexture(gpuChan);
	}
}
