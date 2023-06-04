#include "pch.h"
#include "menu-settings-about.h"
#include "menu-common.h"
#include "constants.h"
#include "scripture.h"
#include "xlations.h"
#include "dialogcontrols.h"

extern bool mainRunning;

namespace blooDot::MenuSettingsAbout
{
	MenuDialogInteraction menuState;
	bool menuRunning = false;

	SDL_Texture* backTexture = NULL;
	std::vector<CreditsMention> creditMentions;
	std::vector<CreditsChapter> creditChapters;
	std::vector<CreditsSection> creditSections;

	SDL_Rect backRect{ 0,0,0,0 };
	std::vector<SDL_Rect> sectionRects;
	std::vector<SDL_Rect> chapterRects;
	std::vector<SDL_Rect> creditsRects;

	bool MenuLoop(SDL_Renderer* renderer)
	{
		menuRunning = true;
		menuState.itemCount = AboutMenuItems::HAM_BACK + 1;
		menuState.selectedItemIndex = AboutMenuItems::HAM_BACK;

		SDL_Rect outerMenuRect{ 150,45,340,390 };
		SDL_Rect titleRect{ 0,0,0,0 };

		const auto titleTexture = RenderText(
			renderer,
			&titleRect,
			FONT_KEY_ALIEN,
			26,
			literalliteralAlienAboutMenuLabel,
			AlienTextColor
		);

		_PrepareText(renderer);

		unsigned short frame = 0;
		while (menuRunning && mainRunning)
		{
			blooDot::MenuCommon::HandleMenu(&menuState);
			if (menuState.leaveDialog || menuState.enterMenuItem)
			{
				/* "back" is the only menu item */
				menuRunning = false;
			}

			if (menuState.leaveMain)
			{
				mainRunning = false;
			}

			if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = IMG_GetError();
				ReportError("Failed to clear the about menu screen", clearError);
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xcf);
			SDL_RenderFillRect(renderer, &outerMenuRect);
			SDL_SetRenderDrawColor(renderer, 0xc5, 0xc5, 0xc5, 0xe9);
			if (SDL_RenderDrawRect(renderer, &outerMenuRect) < 0)
			{
				const auto drawRectError = SDL_GetError();
				ReportError("Failed to draw about menu panel border", drawRectError);
				menuRunning = false;
			};

			DrawLabel(renderer, 286, 51, titleTexture, &titleRect);

			const auto drawingTexture = BeginRenderDrawing(renderer, GodsPreferredWidth, GodsPreferredHight);
			if (drawingTexture) [[likely]]
			{
				const auto drawingSink = GetDrawingSink();
				const int yStart = 94;
				const int stride = 46;
				const int backGap = stride / 2;
				AboutMenuItems itemToDraw = AboutMenuItems::HAM_BACK;
				for (auto y = 94; y < 95; y += stride)
				{
					const auto thisItem = itemToDraw == menuState.selectedItemIndex;
					DrawButton(drawingSink, 195, y, 250, 42, thisItem);
					if (thisItem)
					{
						DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
						DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);
					}

					if (itemToDraw == AboutMenuItems::HAM_BACK)
					{
						y += backGap;
					}

					itemToDraw = static_cast<AboutMenuItems>(itemToDraw + 1);
				}

				EndRenderDrawing(renderer, drawingTexture, nullptr);

				DrawLabel(renderer, 235, yStart + 0 * stride + 0 * backGap, backTexture, &backRect);

				/* the storyboarded "rolling credits */
				const auto& section = creditSections[0];
				SDL_Rect rect = section.rect;
				DrawLabel(renderer, 195, yStart + 1 * stride + 1 * backGap, section.texture, &rect);
				rect = section.chapters[0]->rect;
				DrawLabel(renderer, 10, yStart + 2 * stride + 1 * backGap, section.chapters[0]->texture, &rect);
				rect = section.chapters[0]->mentions[0]->rect;
				DrawLabel(renderer, 195, yStart + 2 * stride + 1 * backGap, section.chapters[0]->mentions[0]->texture, &rect);
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		_Teardown();
		SDL_DestroyTexture(titleTexture);

		return menuRunning;
	}

	void _PrepareText(SDL_Renderer* renderer)
	{
		SDL_Texture* mentionTexture;
		SDL_Texture* chapterTexture;
		SDL_Texture* sectionTexture;
		SDL_Rect textureRect;

		backTexture = RenderText(renderer, &backRect, FONT_KEY_DIALOG, 23, literalMenuBack, ButtonTextColor);
		
		/* 1. build all the distinct mentions that there are */

		mentionTexture = _MentionTexture(literalAboutCreditsMentionDlatikay, &textureRect, renderer);
		_AddMention(mentionTexture, &textureRect);
		const auto& dlatikay = &creditMentions[0];

		//mentionTexture = _MentionTexture(literalAboutCreditsMentionLlatikay, &textureRect, renderer);
		//const auto& llatikay = _AddMention(mentionTexture, &textureRect);

		//mentionTexture = _MentionTexture(literalAboutCreditsMentionJlatikay, &textureRect, renderer);
		//const auto& jlatikay = _AddMention(mentionTexture, &textureRect);

		//mentionTexture = _MentionTexture(literalAboutCreditsMentionNnebel, &textureRect, renderer);
		//const auto& nnebel = _AddMention(mentionTexture, &textureRect);

		/* 2. build the hierarchy */
		sectionTexture = _SectionTexture(literalAboutClassificationProduction, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);

		chapterTexture = _ChapterTexture(literalAboutClassificationProducer, &textureRect, renderer);
		_AddChapter(chapterTexture, &textureRect);
		const auto& producer = &creditChapters[0];
		creditSections[0].chapters.push_back(producer);
		creditSections[0].chapters[0]->mentions.push_back(dlatikay);
	}

	SDL_Texture* _MentionTexture(const char* literal, SDL_Rect* rect, SDL_Renderer* renderer)
	{
		return RenderText(
			renderer,
			rect,
			FONT_KEY_DIALOG,
			15,
			literal,
			ButtonTextColor
		);
	}

	SDL_Texture* _SectionTexture(const char* literal, SDL_Rect* rect, SDL_Renderer* renderer)
	{
		return RenderText(
			renderer,
			rect,
			FONT_KEY_DIALOG,
			28,
			literal,
			ButtonTextColor,
			true
		);
	}

	SDL_Texture* _ChapterTexture(const char* literal, SDL_Rect* rect, SDL_Renderer* renderer)
	{
		return RenderText(
			renderer,
			rect,
			FONT_KEY_DIALOG,
			17,
			literal,
			ChapterTextColor,
			true
		);
	}

	CreditsMention& _AddMention(SDL_Texture* mentionTexture, SDL_Rect* textureRect)
	{
		CreditsMention mention;
		mention.rect = *textureRect;
		mention.texture = mentionTexture;
		
		return creditMentions.emplace_back(mention);
	}

	void _AddSection(SDL_Texture* sectionTexture, SDL_Rect* textureRect)
	{
		CreditsSection section;
		section.rect = *textureRect;
		section.texture = sectionTexture;
		creditSections.emplace_back(section);
	}

	CreditsChapter& _AddChapter(SDL_Texture* chapterTexture, SDL_Rect* textureRect)
	{
		CreditsChapter chapter;
		chapter.rect = *textureRect;
		chapter.texture = chapterTexture;

		return creditChapters.emplace_back(chapter);
	}

	void _Teardown()
	{
		backTexture&& [] { SDL_DestroyTexture(backTexture); return false; }();

		for (const auto& mention : creditMentions)
		{
			if (mention.texture)
			{
				SDL_DestroyTexture(mention.texture);
			}
		}

		creditMentions.clear();
		for (auto& section : creditSections)
		{
			auto& chapters = section.chapters;
			for (const auto& chapter : chapters)
			{
				if (chapter->texture)
				{
					SDL_DestroyTexture(chapter->texture);
				}
			}

			section.chapters.clear();
			if (section.texture)
			{
				SDL_DestroyTexture(section.texture);
			}
		}

		creditSections.clear();
	}
}