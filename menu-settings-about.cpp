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

		mentionTexture = _MentionTexture(literalAboutCreditsMentionMlatikay, &textureRect, renderer);
		const auto& mlatikay = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutCreditsMentionNnebel, &textureRect, renderer);
		const auto& nnebel = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutClassificationOtherTechSDLName, &textureRect, renderer);
		const auto& sdlName = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutClassificationOtherTechSDLCopyr, &textureRect, renderer);
		const auto& sdlCopyr = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutClassificationOtherTechBox2DName, &textureRect, renderer);
		const auto& box2dName = _AddMention(mentionTexture, &textureRect);
		
		mentionTexture = _MentionTexture(literalAboutClassificationOtherTechBox2DCopyr, &textureRect, renderer);
		const auto& box2dCopyr = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutClassificationOtherTechClearSansName, &textureRect, renderer);
		const auto& sansName = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutClassificationOtherTechClearSansCopyr, &textureRect, renderer);
		const auto& sansCopyr = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutClassificationOtherTechFreckleName, &textureRect, renderer);
		const auto& freckleName = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutClassificationOtherTechFreckleCopyr, &textureRect, renderer);
		const auto& freckleCopyr = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutClassificationOtherTechFreckleCopyr, &textureRect, renderer);
		const auto& yesevaName = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutClassificationOtherTechYesevaCopyr, &textureRect, renderer);
		const auto& yesevaCopyr = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutClassificationOtherTechShavianName, &textureRect, renderer);
		const auto& snailName = _AddMention(mentionTexture, &textureRect);

		mentionTexture = _MentionTexture(literalAboutClassificationOtherTechShavianCopyr, &textureRect, renderer);
		const auto& snailCopyr = _AddMention(mentionTexture, &textureRect);

		/* 2. build the hierarchy,
		 * classification "Production" */
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

		chapterTexture = _ChapterTexture(literalAboutClassificationArtGraphicsCharacterDesign, &textureRect, renderer);
		const auto& characterDesign = _AddChapter(chapterTexture, &textureRect);
		characterDesign->mentions.push_back(dlatikay);
		characterDesign->mentions.push_back(llatikay);
		characterDesign->mentions.push_back(jlatikay);
		creditSections[4].chapters.push_back(characterDesign);
		
		chapterTexture = _ChapterTexture(literalAboutClassificationArtGraphicsConceptArt, &textureRect, renderer);
		const auto& conceptArt = _AddChapter(chapterTexture, &textureRect);
		conceptArt->mentions.push_back(llatikay);
		conceptArt->mentions.push_back(jlatikay);
		creditSections[4].chapters.push_back(conceptArt);
		
		chapterTexture = _ChapterTexture(literalAboutClassificationArtGraphicsIllustration, &textureRect, renderer);
		const auto& illustrations = _AddChapter(chapterTexture, &textureRect);
		illustrations->mentions.push_back(nnebel);
		creditSections[4].chapters.push_back(illustrations);
		
		chapterTexture = _ChapterTexture(literalAboutClassificationArtGraphicsStoryboard, &textureRect, renderer);
		const auto& storyboard = _AddChapter(chapterTexture, &textureRect);
		storyboard->mentions.push_back(dlatikay);
		storyboard->mentions.push_back(llatikay);
		storyboard->mentions.push_back(jlatikay);
		creditSections[4].chapters.push_back(storyboard);
		
		chapterTexture = _ChapterTexture(literalAboutClassificationArtGraphicsMenuHud, &textureRect, renderer);
		const auto& menuHud = _AddChapter(chapterTexture, &textureRect);
		menuHud->mentions.push_back(dlatikay);
		creditSections[4].chapters.push_back(menuHud);

		/* classification "Audio" */
		sectionTexture = _SectionTexture(literalAboutClassificationAudio, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);

		chapterTexture = _ChapterTexture(literalAboutClassificationAudioComposer, &textureRect, renderer);
		const auto& composer = _AddChapter(chapterTexture, &textureRect);
		composer->mentions.push_back(dlatikay);
		creditSections[5].chapters.push_back(composer);
		
		chapterTexture = _ChapterTexture(literalAboutClassificationAudioSfx, &textureRect, renderer);
		const auto& audioSfx = _AddChapter(chapterTexture, &textureRect);
		audioSfx->mentions.push_back(dlatikay);
		creditSections[5].chapters.push_back(audioSfx);

		/* classification "Quality Assurance" */
		sectionTexture = _SectionTexture(literalAboutClassificationQualityAssurance, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);

		chapterTexture = _ChapterTexture(literalAboutClassificationQualityAssuranceTesters, &textureRect, renderer);
		const auto& testers = _AddChapter(chapterTexture, &textureRect);
		testers->mentions.push_back(llatikay);
		creditSections[6].chapters.push_back(testers);
		
		chapterTexture = _ChapterTexture(literalAboutClassificationQualityAssurancePlaytesting, &textureRect, renderer);
		const auto& playtesting = _AddChapter(chapterTexture, &textureRect);
		playtesting->mentions.push_back(jlatikay);
		creditSections[6].chapters.push_back(playtesting);
		
		chapterTexture = _ChapterTexture(literalAboutClassificationQualityAssuranceCompatibility, &textureRect, renderer);
		const auto& compatibility = _AddChapter(chapterTexture, &textureRect);
		compatibility->mentions.push_back(dlatikay);
		creditSections[6].chapters.push_back(compatibility);

		/* classification "Localization" */
		sectionTexture = _SectionTexture(literalAboutClassificationLocalization, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);

		chapterTexture = _ChapterTexture(literalAboutClassificationLocalizationAm, &textureRect, renderer);
		const auto& am = _AddChapter(chapterTexture, &textureRect);
		am->mentions.push_back(dlatikay);
		creditSections[7].chapters.push_back(am);

		chapterTexture = _ChapterTexture(literalAboutClassificationLocalizationDe, &textureRect, renderer);
		const auto& de = _AddChapter(chapterTexture, &textureRect);
		de->mentions.push_back(dlatikay);
		creditSections[7].chapters.push_back(de);
				
		chapterTexture = _ChapterTexture(literalAboutClassificationLocalizationFi, &textureRect, renderer);
		const auto& fi = _AddChapter(chapterTexture, &textureRect);
		fi->mentions.push_back(dlatikay);
		creditSections[7].chapters.push_back(fi);
		
		chapterTexture = _ChapterTexture(literalAboutClassificationLocalizationUa, &textureRect, renderer);
		const auto& ua = _AddChapter(chapterTexture, &textureRect);
		ua->mentions.push_back(dlatikay);
		creditSections[7].chapters.push_back(ua);
		
		chapterTexture = _ChapterTexture(literalAboutClassificationLocalizationSnailian, &textureRect, renderer);
		const auto& snailian = _AddChapter(chapterTexture, &textureRect);
		snailian->mentions.push_back(dlatikay);
		creditSections[7].chapters.push_back(snailian);
		
		chapterTexture = _ChapterTexture(literalAboutClassificationLocalizationTranslation, &textureRect, renderer);
		const auto& translation = _AddChapter(chapterTexture, &textureRect);
		translation->mentions.push_back(dlatikay);
		creditSections[7].chapters.push_back(translation);

		/* classification "Creative Services" */
		sectionTexture = _SectionTexture(literalAboutClassificationCreativeServices, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);

		chapterTexture = _ChapterTexture(literalAboutClassificationCreativeServicesCoverArt, &textureRect, renderer);
		const auto& coverArt = _AddChapter(chapterTexture, &textureRect);
		coverArt->mentions.push_back(nnebel);
		creditSections[8].chapters.push_back(coverArt);
		
		/* classification "Thanks" */
		sectionTexture = _SectionTexture(literalAboutClassificationThanks, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);

		chapterTexture = _ChapterTexture(literalAboutClassificationThanksDedicatedTo, &textureRect, renderer);
		const auto& dedication = _AddChapter(chapterTexture, &textureRect);
		dedication->mentions.push_back(llatikay);
		dedication->mentions.push_back(jlatikay);
		creditSections[9].chapters.push_back(dedication);
		
		chapterTexture = _ChapterTexture(literalAboutClassificationThanksInMemoryOf, &textureRect, renderer);
		const auto& inMemory = _AddChapter(chapterTexture, &textureRect);
		inMemory->mentions.push_back(llatikay);
		inMemory->mentions.push_back(jlatikay);
		creditSections[9].chapters.push_back(inMemory);
		
		chapterTexture = _ChapterTexture(literalAboutClassificationThanksSpecial, &textureRect, renderer);
		const auto& specialThanks = _AddChapter(chapterTexture, &textureRect);
		specialThanks->mentions.push_back(nnebel);
		creditSections[9].chapters.push_back(specialThanks);

		/* classification "Other" */
		sectionTexture = _SectionTexture(literalAboutClassificationOtherTech, &textureRect, renderer);
		_AddSection(sectionTexture, &textureRect);

		chapterTexture = _ChapterTexture(literalAboutClassificationOtherTechSDL, &textureRect, renderer);
		const auto& techSdl = _AddChapter(chapterTexture, &textureRect);
		techSdl->mentions.push_back(sdlName);
		techSdl->mentions.push_back(sdlCopyr);
		creditSections[10].chapters.push_back(techSdl);

		chapterTexture = _ChapterTexture(literalAboutClassificationOtherTechBox2D, &textureRect, renderer);
		const auto& techBox2d = _AddChapter(chapterTexture, &textureRect);
		techBox2d->mentions.push_back(box2dName);
		techBox2d->mentions.push_back(box2dCopyr);
		creditSections[10].chapters.push_back(techBox2d);

		chapterTexture = _ChapterTexture(literalAboutClassificationOtherTechClearSans, &textureRect, renderer);
		const auto& techClearSans = _AddChapter(chapterTexture, &textureRect);
		techClearSans->mentions.push_back(sansName);
		techClearSans->mentions.push_back(sansCopyr);
		creditSections[10].chapters.push_back(techClearSans);

		chapterTexture = _ChapterTexture(literalAboutClassificationOtherTechFreckle, &textureRect, renderer);
		const auto& techFreckle = _AddChapter(chapterTexture, &textureRect);
		techFreckle->mentions.push_back(freckleName);
		techFreckle->mentions.push_back(freckleCopyr);
		creditSections[10].chapters.push_back(techFreckle);

		chapterTexture = _ChapterTexture(literalAboutClassificationOtherTechYeseva, &textureRect, renderer);
		const auto& techYeseva = _AddChapter(chapterTexture, &textureRect);
		techYeseva->mentions.push_back(yesevaName);
		techYeseva->mentions.push_back(yesevaCopyr);
		creditSections[10].chapters.push_back(techYeseva);

		chapterTexture = _ChapterTexture(literalAboutClassificationOtherTechShavian, &textureRect, renderer);
		const auto& techSnailian = _AddChapter(chapterTexture, &textureRect);
		techSnailian->mentions.push_back(snailName);
		techSnailian->mentions.push_back(snailCopyr);
		creditSections[10].chapters.push_back(techSnailian);
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