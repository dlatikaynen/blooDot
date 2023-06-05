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
	std::vector<std::shared_ptr<CreditsMention>> creditMentions;
	std::vector<std::shared_ptr<CreditsChapter>> creditChapters;
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
		const auto& dlatikay = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutCreditsMentionLlatikay, &textureRect, renderer);
		const auto& llatikay = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutCreditsMentionJlatikay, &textureRect, renderer);
		const auto& jlatikay = _AddMention(mentionTexture, &textureRect);

		// TODO: Max
		mentionTexture = _MentionTexture(literalAboutCreditsMentionJlatikay, &textureRect, renderer);
		const auto& mlatikay = _AddMention(mentionTexture, &textureRect);

		//mentionTexture = _MentionTexture(literalAboutCreditsMentionNnebel, &textureRect, renderer);
		//const auto& nnebel = _AddMention(mentionTexture, &textureRect);

		/* 2. build the hierarchy,
		 * classification "production" */
		sectionTexture = _SectionTexture(literalAboutClassificationProduction, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);

		chapterTexture = _ChapterTexture(literalAboutClassificationProducer, &textureRect, renderer);
		const auto& producer = _AddChapter(chapterTexture, &textureRect);
		producer->mentions.push_back(dlatikay);
		creditSections[0].chapters.push_back(producer);

		chapterTexture = _ChapterTexture(literalAboutClassificationProducerJunior, &textureRect, renderer);
		const auto& producerJunior = _AddChapter(chapterTexture, &textureRect);
		producerJunior->mentions.push_back(llatikay);
		producerJunior->mentions.push_back(jlatikay);
		creditSections[0].chapters.push_back(producerJunior);

		chapterTexture = _ChapterTexture(literalAboutClassificationSupervisor, &textureRect, renderer);
		const auto& supervisor = _AddChapter(chapterTexture, &textureRect);
		producerJunior->mentions.push_back(mlatikay);
		creditSections[0].chapters.push_back(supervisor);

		/* classification "Design" */
		sectionTexture = _SectionTexture(literalAboutClassificationDesign, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);

		chapterTexture = _ChapterTexture(literalAboutClassificationDesignOriginal, &textureRect, renderer);
		const auto& designerOriginal = _AddChapter(chapterTexture, &textureRect);
		designerOriginal->mentions.push_back(dlatikay);
		creditSections[1].chapters.push_back(designerOriginal);

		chapterTexture = _ChapterTexture(literalAboutClassificationDesignAdditionalMiniboss, &textureRect, renderer);
		const auto& additionalMiniboss = _AddChapter(chapterTexture, &textureRect);
		additionalMiniboss->mentions.push_back(jlatikay);
		additionalMiniboss->mentions.push_back(llatikay);
		creditSections[1].chapters.push_back(additionalMiniboss);

		chapterTexture = _ChapterTexture(literalAboutClassificationDesignQuest, &textureRect, renderer);
		const auto& designedQuest = _AddChapter(chapterTexture, &textureRect);
		designedQuest->mentions.push_back(dlatikay);
		designedQuest->mentions.push_back(llatikay);
		designedQuest->mentions.push_back(jlatikay);
		creditSections[1].chapters.push_back(designedQuest);

		/* classification "Programming" */
		sectionTexture = _SectionTexture(literalAboutClassificationProgramming, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);

		chapterTexture = _ChapterTexture(literalAboutClassificationProgrammingEngineering, &textureRect, renderer);
		const auto& engineering = _AddChapter(chapterTexture, &textureRect);
		engineering->mentions.push_back(dlatikay);
		creditSections[2].chapters.push_back(engineering);

		chapterTexture = _ChapterTexture(literalAboutClassificationProgrammingAdditional, &textureRect, renderer);
		const auto& programmingAdditional = _AddChapter(chapterTexture, &textureRect);
		programmingAdditional->mentions.push_back(llatikay);
		creditSections[2].chapters.push_back(programmingAdditional);

		chapterTexture = _ChapterTexture(literalAboutClassificationProgrammingRendering, &textureRect, renderer);
		const auto& rendering = _AddChapter(chapterTexture, &textureRect);
		rendering->mentions.push_back(dlatikay);
		creditSections[2].chapters.push_back(rendering);

		chapterTexture = _ChapterTexture(literalAboutClassificationProgrammingPhysics, &textureRect, renderer);
		const auto& physics = _AddChapter(chapterTexture, &textureRect);
		physics->mentions.push_back(dlatikay);
		creditSections[2].chapters.push_back(physics);

		/* classification "Writing" */
		sectionTexture = _SectionTexture(literalAboutClassificationWriters, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);

		chapterTexture = _ChapterTexture(literalAboutClassificationWritersStory, &textureRect, renderer);
		const auto& story = _AddChapter(chapterTexture, &textureRect);
		story->mentions.push_back(dlatikay);
		creditSections[3].chapters.push_back(story);

		chapterTexture = _ChapterTexture(literalAboutClassificationWritersWorldbuilding, &textureRect, renderer);
		const auto& worldbuilding = _AddChapter(chapterTexture, &textureRect);
		worldbuilding->mentions.push_back(dlatikay);
		worldbuilding->mentions.push_back(jlatikay);
		creditSections[3].chapters.push_back(worldbuilding);

		chapterTexture = _ChapterTexture(literalAboutClassificationWritersDialogue, &textureRect, renderer);
		const auto& dialogue = _AddChapter(chapterTexture, &textureRect);
		dialogue->mentions.push_back(dlatikay);
		creditSections[3].chapters.push_back(dialogue);

		/* classification "Graphics" */
		sectionTexture = _SectionTexture(literalAboutClassificationArtGraphics, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);
//literalAboutClassificationArtGraphicsCharacterDesign = "";
//literalAboutClassificationArtGraphicsConceptArt = "";
//literalAboutClassificationArtGraphicsIllustration = "";
//literalAboutClassificationArtGraphicsStoryboard = "";
//literalAboutClassificationArtGraphicsMenuHud = "";

		/* classification "Audio" */
		sectionTexture = _SectionTexture(literalAboutClassificationAudio, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);
//literalAboutClassificationAudioComposer = "";
//literalAboutClassificationAudioSfx = "";

		/* classification "Quality Assurance" */
		sectionTexture = _SectionTexture(literalAboutClassificationQualityAssurance, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);
//literalAboutClassificationQualityAssuranceTesters = "";
//literalAboutClassificationQualityAssurancePlaytesting = "";
//literalAboutClassificationQualityAssuranceCompatibility = "";

		/* classification "Localization" */
		sectionTexture = _SectionTexture(literalAboutClassificationLocalization, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);
//literalAboutClassificationLocalizationDe = "";
//literalAboutClassificationLocalizationAm = "";
//literalAboutClassificationLocalizationFi = "";
//literalAboutClassificationLocalizationUa = "";
//literalAboutClassificationLocalizationSnailian = "";
//literalAboutClassificationLocalizationTranslation = "";

		/* classification "Creative Services" */
		sectionTexture = _SectionTexture(literalAboutClassificationCreativeServices, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);
//literalAboutClassificationCreativeServicesCoverArt = "";
		
		/* classification "Thanks" */
		sectionTexture = _SectionTexture(literalAboutClassificationThanks, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);
//literalAboutClassificationThanksDedicatedTo = "";
//literalAboutClassificationThanksInMemoryOf = "";
//literalAboutClassificationThanksSpecial = "";

		/* classification "Other" */
		sectionTexture = _SectionTexture(literalAboutClassificationOther, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);
//literalAboutClassificationOtherTech = "";
//literalAboutClassificationOtherTechSDL = "";
//literalAboutClassificationOtherTechSDLCopyr = "";
//literalAboutClassificationOtherTechBox2D = "";
//literalAboutClassificationOtherTechBox2DCopyr = "";
//literalAboutClassificationOtherTechClearSans = "";
//literalAboutClassificationOtherTechFreckle = "";
//literalAboutClassificationOtherTechFreckleCopyr = "";
//literalAboutClassificationOtherTechYeseva = "";
//literalAboutClassificationOtherTechYesevaCopyr = "";
//literalAboutClassificationOtherTechShavian = "";
//literalAboutClassificationOtherTechShavianCopyr = "";
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

	std::shared_ptr<CreditsMention> _AddMention(SDL_Texture* mentionTexture, SDL_Rect* textureRect)
	{
		auto mention = std::make_shared<CreditsMention>();
		mention->rect = *textureRect;
		mention->texture = mentionTexture;
		creditMentions.push_back(mention);

		return mention;
	}

	void _AddSection(SDL_Texture* sectionTexture, SDL_Rect* textureRect)
	{
		CreditsSection section;
		section.rect = *textureRect;
		section.texture = sectionTexture;
		creditSections.emplace_back(section);
	}

	std::shared_ptr<CreditsChapter> _AddChapter(SDL_Texture* chapterTexture, SDL_Rect* textureRect)
	{
		auto chapter = std::make_shared<CreditsChapter>();
		chapter->rect = *textureRect;
		chapter->texture = chapterTexture;
		creditChapters.push_back(chapter);

		return chapter;
	}

	void _Teardown()
	{
		backTexture&& [] { SDL_DestroyTexture(backTexture); return false; }();

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
		creditChapters.clear();
		for (const auto& mention : creditMentions)
		{
			if (mention->texture)
			{
				SDL_DestroyTexture(mention->texture);
			}
		}

		creditMentions.clear();
	}
}