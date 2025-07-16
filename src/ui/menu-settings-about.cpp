#include "menu-settings-about.h"

#include "dialog-controls.h"
#include "drawing.h"
#include "menu-common.h"
#include "scripture.h"
#include "layout-constants.h"
#include "../../main.h"
#include "../../res/xlations.h"

namespace blooDot::MenuSettingsAbout
{
	MenuCommon::MenuDialogInteraction menuState;
	bool menuRunning = false;

	SDL_Texture* backTexture = nullptr;
	std::vector<std::shared_ptr<CreditsMention>> creditMentions;
	std::vector<std::shared_ptr<CreditsChapter>> creditChapters;
	std::vector<CreditsSection> creditSections;

	SDL_FRect backRect{ 0,0,0,0 };
	std::vector<SDL_FRect> sectionRects;
	std::vector<SDL_FRect> chapterRects;
	std::vector<SDL_FRect> creditsRects;

	bool MenuLoop(SDL_Renderer* renderer)
	{
		menuRunning = true;
		menuState.itemCount = Constants::AboutMenuItems::HAM_BACK + 1;
		menuState.selectedItemIndex = Constants::AboutMenuItems::HAM_BACK;

		SDL_FRect outerMenuRect{ 150,45,340,390 };
		SDL_FRect titleRect{ 0,0,0,0 };

		const auto titleTexture = Scripture::RenderText(
			renderer,
			&titleRect,
			Scripture::FONT_KEY_ALIEN,
			Ui::MenuPanelTitleFontSize,
			literalAlienAboutMenuLabel,
			Constants::AlienTextColor
		);

		PrepareTextInternal(renderer);

		unsigned short frame = 0;
		while (menuRunning) // TODO:  && mainRunning)
		{
			blooDot::MenuCommon::HandleMenu(&menuState);
			if (menuState.leaveDialog || menuState.enterMenuItem)
			{
				/* "back" is the only menu item */
				menuRunning = false;
			}

			if (menuState.leaveMain)
			{
				// TODO: mainRunning = false;
			}

			if (SDL_RenderClear(renderer) < 0)
			{
				const auto clearError = SDL_GetError();

				ReportError("Failed to clear the about menu screen", clearError);
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			if (!MenuCommon::DrawMenuPanel(renderer, &outerMenuRect, titleTexture, &titleRect))
			{
				menuRunning = false;
			};

			if (const auto drawingTexture = Drawing::BeginRenderDrawing(renderer, Constants::GodsPreferredWidth, Constants::GodsPreferredHight)) [[likely]]
			{
				const auto drawingSink = Drawing::GetDrawingSink();
				constexpr int yStart = 94;
				constexpr int stride = 46;
				constexpr int backGap = stride / 2;
				Constants::AboutMenuItems itemToDraw = Constants::AboutMenuItems::HAM_BACK;

				auto y = 94;
				for (; y < 95; y += stride)
				{
					const auto thisItem = itemToDraw == menuState.selectedItemIndex;
					DialogControls::DrawButton(drawingSink, 195, y, 250, 42, thisItem ? Constants::CH_MAINMENU : Constants::CH_NONE);
					if (thisItem)
					{
						DialogControls::DrawChevron(drawingSink, 195 - 7, y + 21, false, frame);
						DialogControls::DrawChevron(drawingSink, 195 + 250 + 7, y + 21, true, frame);
					}

					if (itemToDraw == Constants::AboutMenuItems::HAM_BACK)
					{
						y += backGap;
					}

					itemToDraw = static_cast<Constants::AboutMenuItems>(itemToDraw + 1);
				}

				DialogControls::DrawButton(drawingSink, 162, y, 316, 258, Constants::CH_NONE);
				Drawing::EndRenderDrawing(renderer, drawingTexture, nullptr);
				DialogControls::DrawLabel(renderer, 235, yStart + 0 * stride + 0 * backGap, backTexture, &backRect);

				/* the storyboarded rolling credits */
				y = backGap;

				for (const auto& section: creditSections) {
					SDL_FRect rect = section.rect;

					DialogControls::DrawLabel(renderer, 195, y, section.texture, &rect);
					y += static_cast<int>(rect.h);
					for (const auto& chapter: section.chapters) {
						rect = chapter->rect;
						DialogControls::DrawLabel(renderer, 10, y, chapter->texture, &rect);
						for (const auto& mention: chapter->mentions) {
							rect = mention->rect;
							DialogControls::DrawLabel(renderer, 195, y, mention->texture, &rect);
							y += static_cast<int>(rect.h);
						}
					}
				}
			}

			SDL_RenderPresent(renderer);
			SDL_Delay(16);
			++frame;
		}

		TeardownInternal();
		SDL_DestroyTexture(titleTexture);

		return menuRunning;
	}

	void PrepareTextInternal(SDL_Renderer* renderer)
	{
		SDL_Texture* mentionTexture;
		SDL_Texture* chapterTexture;
		SDL_Texture* sectionTexture;
		SDL_FRect textureRect;

		backTexture = Scripture::RenderText(renderer, &backRect, Scripture::FONT_KEY_DIALOG, 23, literalMenuBack, Constants::ButtonTextColor);
		
		/* 1. build all the distinct mentions that there are */
		mentionTexture = MentionTextureInternal(literalAboutCreditsMentionDlatikay, &textureRect, renderer);
		const auto& dlatikay = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutCreditsMentionLlatikay, &textureRect, renderer);
		const auto& llatikay = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutCreditsMentionJlatikay, &textureRect, renderer);
		const auto& jlatikay = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutCreditsMentionMlatikay, &textureRect, renderer);
		const auto& mlatikay = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutCreditsMentionNnebel, &textureRect, renderer);
		const auto& nnebel = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutClassificationOtherTechSDLName, &textureRect, renderer);
		const auto& sdlName = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutClassificationOtherTechSDLCopyr, &textureRect, renderer);
		const auto& sdlCopyr = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutClassificationOtherTechBox2DName, &textureRect, renderer);
		const auto& box2dName = AddMentionInternal(mentionTexture, &textureRect);
		
		mentionTexture = MentionTextureInternal(literalAboutClassificationOtherTechBox2DCopyr, &textureRect, renderer);
		const auto& box2dCopyr = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutClassificationOtherTechClearSansName, &textureRect, renderer);
		const auto& sansName = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutClassificationOtherTechClearSansCopyr, &textureRect, renderer);
		const auto& sansCopyr = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutClassificationOtherTechFreckleName, &textureRect, renderer);
		const auto& freckleName = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutClassificationOtherTechFreckleCopyr, &textureRect, renderer);
		const auto& freckleCopyr = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutClassificationOtherTechFreckleCopyr, &textureRect, renderer);
		const auto& yesevaName = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutClassificationOtherTechYesevaCopyr, &textureRect, renderer);
		const auto& yesevaCopyr = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutClassificationOtherTechShavianName, &textureRect, renderer);
		const auto& snailName = AddMentionInternal(mentionTexture, &textureRect);

		mentionTexture = MentionTextureInternal(literalAboutClassificationOtherTechShavianCopyr, &textureRect, renderer);
		const auto& snailCopyr = AddMentionInternal(mentionTexture, &textureRect);

		/* 2. build the hierarchy,
		 * classification "Production" */
		sectionTexture = SectionTextureInternal(literalAboutClassificationProduction, &textureRect, renderer);
		AddSectionInternal(sectionTexture, &textureRect);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationProducer, &textureRect, renderer);
		const auto& producer = AddChapterInternal(chapterTexture, &textureRect);
		producer->mentions.push_back(dlatikay);
		creditSections[0].chapters.push_back(producer);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationProducerJunior, &textureRect, renderer);
		const auto& producerJunior = AddChapterInternal(chapterTexture, &textureRect);
		producerJunior->mentions.push_back(llatikay);
		producerJunior->mentions.push_back(jlatikay);
		creditSections[0].chapters.push_back(producerJunior);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationSupervisor, &textureRect, renderer);
		const auto& supervisor = AddChapterInternal(chapterTexture, &textureRect);
		producerJunior->mentions.push_back(mlatikay);
		creditSections[0].chapters.push_back(supervisor);

		/* classification "Design" */
		sectionTexture = SectionTextureInternal(literalAboutClassificationDesign, &textureRect, renderer);
		AddSectionInternal(sectionTexture, &textureRect);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationDesignOriginal, &textureRect, renderer);
		const auto& designerOriginal = AddChapterInternal(chapterTexture, &textureRect);
		designerOriginal->mentions.push_back(dlatikay);
		creditSections[1].chapters.push_back(designerOriginal);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationDesignAdditionalMiniboss, &textureRect, renderer);
		const auto& additionalMiniboss = AddChapterInternal(chapterTexture, &textureRect);
		additionalMiniboss->mentions.push_back(jlatikay);
		additionalMiniboss->mentions.push_back(llatikay);
		creditSections[1].chapters.push_back(additionalMiniboss);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationDesignQuest, &textureRect, renderer);
		const auto& designedQuest = AddChapterInternal(chapterTexture, &textureRect);
		designedQuest->mentions.push_back(dlatikay);
		designedQuest->mentions.push_back(llatikay);
		designedQuest->mentions.push_back(jlatikay);
		creditSections[1].chapters.push_back(designedQuest);

		/* classification "Programming" */
		sectionTexture = SectionTextureInternal(literalAboutClassificationProgramming, &textureRect, renderer);
		AddSectionInternal(sectionTexture, &textureRect);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationProgrammingEngineering, &textureRect, renderer);
		const auto& engineering = AddChapterInternal(chapterTexture, &textureRect);
		engineering->mentions.push_back(dlatikay);
		creditSections[2].chapters.push_back(engineering);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationProgrammingAdditional, &textureRect, renderer);
		const auto& programmingAdditional = AddChapterInternal(chapterTexture, &textureRect);
		programmingAdditional->mentions.push_back(llatikay);
		creditSections[2].chapters.push_back(programmingAdditional);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationProgrammingRendering, &textureRect, renderer);
		const auto& rendering = AddChapterInternal(chapterTexture, &textureRect);
		rendering->mentions.push_back(dlatikay);
		creditSections[2].chapters.push_back(rendering);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationProgrammingPhysics, &textureRect, renderer);
		const auto& physics = AddChapterInternal(chapterTexture, &textureRect);
		physics->mentions.push_back(dlatikay);
		creditSections[2].chapters.push_back(physics);

		/* classification "Writing" */
		sectionTexture = SectionTextureInternal(literalAboutClassificationWriters, &textureRect, renderer);
		AddSectionInternal(sectionTexture, &textureRect);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationWritersStory, &textureRect, renderer);
		const auto& story = AddChapterInternal(chapterTexture, &textureRect);
		story->mentions.push_back(dlatikay);
		creditSections[3].chapters.push_back(story);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationWritersWorldbuilding, &textureRect, renderer);
		const auto& worldbuilding = AddChapterInternal(chapterTexture, &textureRect);
		worldbuilding->mentions.push_back(dlatikay);
		worldbuilding->mentions.push_back(jlatikay);
		creditSections[3].chapters.push_back(worldbuilding);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationWritersDialogue, &textureRect, renderer);
		const auto& dialogue = AddChapterInternal(chapterTexture, &textureRect);
		dialogue->mentions.push_back(dlatikay);
		creditSections[3].chapters.push_back(dialogue);

		/* classification "Graphics" */
		sectionTexture = SectionTextureInternal(literalAboutClassificationArtGraphics, &textureRect, renderer);
		AddSectionInternal(sectionTexture, &textureRect);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationArtGraphicsCharacterDesign, &textureRect, renderer);
		const auto& characterDesign = AddChapterInternal(chapterTexture, &textureRect);
		characterDesign->mentions.push_back(dlatikay);
		characterDesign->mentions.push_back(llatikay);
		characterDesign->mentions.push_back(jlatikay);
		creditSections[4].chapters.push_back(characterDesign);
		
		chapterTexture = ChapterTextureInternal(literalAboutClassificationArtGraphicsConceptArt, &textureRect, renderer);
		const auto& conceptArt = AddChapterInternal(chapterTexture, &textureRect);
		conceptArt->mentions.push_back(llatikay);
		conceptArt->mentions.push_back(jlatikay);
		creditSections[4].chapters.push_back(conceptArt);
		
		chapterTexture = ChapterTextureInternal(literalAboutClassificationArtGraphicsIllustration, &textureRect, renderer);
		const auto& illustrations = AddChapterInternal(chapterTexture, &textureRect);
		illustrations->mentions.push_back(nnebel);
		creditSections[4].chapters.push_back(illustrations);
		
		chapterTexture = ChapterTextureInternal(literalAboutClassificationArtGraphicsStoryboard, &textureRect, renderer);
		const auto& storyboard = AddChapterInternal(chapterTexture, &textureRect);
		storyboard->mentions.push_back(dlatikay);
		storyboard->mentions.push_back(llatikay);
		storyboard->mentions.push_back(jlatikay);
		creditSections[4].chapters.push_back(storyboard);
		
		chapterTexture = ChapterTextureInternal(literalAboutClassificationArtGraphicsMenuHud, &textureRect, renderer);
		const auto& menuHud = AddChapterInternal(chapterTexture, &textureRect);
		menuHud->mentions.push_back(dlatikay);
		creditSections[4].chapters.push_back(menuHud);

		/* classification "Audio" */
		sectionTexture = SectionTextureInternal(literalAboutClassificationAudio, &textureRect, renderer);
		AddSectionInternal(sectionTexture, &textureRect);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationAudioComposer, &textureRect, renderer);
		const auto& composer = AddChapterInternal(chapterTexture, &textureRect);
		composer->mentions.push_back(dlatikay);
		creditSections[5].chapters.push_back(composer);
		
		chapterTexture = ChapterTextureInternal(literalAboutClassificationAudioSfx, &textureRect, renderer);
		const auto& audioSfx = AddChapterInternal(chapterTexture, &textureRect);
		audioSfx->mentions.push_back(dlatikay);
		creditSections[5].chapters.push_back(audioSfx);

		/* classification "Quality Assurance" */
		sectionTexture = SectionTextureInternal(literalAboutClassificationQualityAssurance, &textureRect, renderer);
		AddSectionInternal(sectionTexture, &textureRect);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationQualityAssuranceTesters, &textureRect, renderer);
		const auto& testers = AddChapterInternal(chapterTexture, &textureRect);
		testers->mentions.push_back(llatikay);
		creditSections[6].chapters.push_back(testers);
		
		chapterTexture = ChapterTextureInternal(literalAboutClassificationQualityAssurancePlaytesting, &textureRect, renderer);
		const auto& playtesting = AddChapterInternal(chapterTexture, &textureRect);
		playtesting->mentions.push_back(jlatikay);
		creditSections[6].chapters.push_back(playtesting);
		
		chapterTexture = ChapterTextureInternal(literalAboutClassificationQualityAssuranceCompatibility, &textureRect, renderer);
		const auto& compatibility = AddChapterInternal(chapterTexture, &textureRect);
		compatibility->mentions.push_back(dlatikay);
		creditSections[6].chapters.push_back(compatibility);

		/* classification "Localization" */
		sectionTexture = SectionTextureInternal(literalAboutClassificationLocalization, &textureRect, renderer);
		AddSectionInternal(sectionTexture, &textureRect);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationLocalizationAm, &textureRect, renderer);
		const auto& am = AddChapterInternal(chapterTexture, &textureRect);
		am->mentions.push_back(dlatikay);
		creditSections[7].chapters.push_back(am);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationLocalizationDe, &textureRect, renderer);
		const auto& de = AddChapterInternal(chapterTexture, &textureRect);
		de->mentions.push_back(dlatikay);
		creditSections[7].chapters.push_back(de);
				
		chapterTexture = ChapterTextureInternal(literalAboutClassificationLocalizationFi, &textureRect, renderer);
		const auto& fi = AddChapterInternal(chapterTexture, &textureRect);
		fi->mentions.push_back(dlatikay);
		creditSections[7].chapters.push_back(fi);
		
		chapterTexture = ChapterTextureInternal(literalAboutClassificationLocalizationUa, &textureRect, renderer);
		const auto& ua = AddChapterInternal(chapterTexture, &textureRect);
		ua->mentions.push_back(dlatikay);
		creditSections[7].chapters.push_back(ua);
		
		chapterTexture = ChapterTextureInternal(literalAboutClassificationLocalizationSnailian, &textureRect, renderer);
		const auto& snailian = AddChapterInternal(chapterTexture, &textureRect);
		snailian->mentions.push_back(dlatikay);
		creditSections[7].chapters.push_back(snailian);
		
		chapterTexture = ChapterTextureInternal(literalAboutClassificationLocalizationTranslation, &textureRect, renderer);
		const auto& translation = AddChapterInternal(chapterTexture, &textureRect);
		translation->mentions.push_back(dlatikay);
		creditSections[7].chapters.push_back(translation);

		/* classification "Creative Services" */
		sectionTexture = SectionTextureInternal(literalAboutClassificationCreativeServices, &textureRect, renderer);
		AddSectionInternal(sectionTexture, &textureRect);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationCreativeServicesCoverArt, &textureRect, renderer);
		const auto& coverArt = AddChapterInternal(chapterTexture, &textureRect);
		coverArt->mentions.push_back(nnebel);
		creditSections[8].chapters.push_back(coverArt);
		
		/* classification "Thanks" */
		sectionTexture = SectionTextureInternal(literalAboutClassificationThanks, &textureRect, renderer);
		AddSectionInternal(sectionTexture, &textureRect);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationThanksDedicatedTo, &textureRect, renderer);
		const auto& dedication = AddChapterInternal(chapterTexture, &textureRect);
		dedication->mentions.push_back(llatikay);
		dedication->mentions.push_back(jlatikay);
		creditSections[9].chapters.push_back(dedication);
		
		chapterTexture = ChapterTextureInternal(literalAboutClassificationThanksInMemoryOf, &textureRect, renderer);
		const auto& inMemory = AddChapterInternal(chapterTexture, &textureRect);
		inMemory->mentions.push_back(llatikay);
		inMemory->mentions.push_back(jlatikay);
		creditSections[9].chapters.push_back(inMemory);
		
		chapterTexture = ChapterTextureInternal(literalAboutClassificationThanksSpecial, &textureRect, renderer);
		const auto& specialThanks = AddChapterInternal(chapterTexture, &textureRect);
		specialThanks->mentions.push_back(nnebel);
		creditSections[9].chapters.push_back(specialThanks);

		/* classification "Other" */
		sectionTexture = SectionTextureInternal(literalAboutClassificationOtherTech, &textureRect, renderer);
		AddSectionInternal(sectionTexture, &textureRect);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationOtherTechSDL, &textureRect, renderer);
		const auto& techSdl = AddChapterInternal(chapterTexture, &textureRect);
		techSdl->mentions.push_back(sdlName);
		techSdl->mentions.push_back(sdlCopyr);
		creditSections[10].chapters.push_back(techSdl);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationOtherTechBox2D, &textureRect, renderer);
		const auto& techBox2d = AddChapterInternal(chapterTexture, &textureRect);
		techBox2d->mentions.push_back(box2dName);
		techBox2d->mentions.push_back(box2dCopyr);
		creditSections[10].chapters.push_back(techBox2d);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationOtherTechClearSans, &textureRect, renderer);
		const auto& techClearSans = AddChapterInternal(chapterTexture, &textureRect);
		techClearSans->mentions.push_back(sansName);
		techClearSans->mentions.push_back(sansCopyr);
		creditSections[10].chapters.push_back(techClearSans);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationOtherTechFreckle, &textureRect, renderer);
		const auto& techFreckle = AddChapterInternal(chapterTexture, &textureRect);
		techFreckle->mentions.push_back(freckleName);
		techFreckle->mentions.push_back(freckleCopyr);
		creditSections[10].chapters.push_back(techFreckle);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationOtherTechYeseva, &textureRect, renderer);
		const auto& techYeseva = AddChapterInternal(chapterTexture, &textureRect);
		techYeseva->mentions.push_back(yesevaName);
		techYeseva->mentions.push_back(yesevaCopyr);
		creditSections[10].chapters.push_back(techYeseva);

		chapterTexture = ChapterTextureInternal(literalAboutClassificationOtherTechShavian, &textureRect, renderer);
		const auto& techSnailian = AddChapterInternal(chapterTexture, &textureRect);
		techSnailian->mentions.push_back(snailName);
		techSnailian->mentions.push_back(snailCopyr);
		creditSections[10].chapters.push_back(techSnailian);
	}

	SDL_Texture* MentionTextureInternal(const char* literal, SDL_FRect* rect, SDL_Renderer* renderer)
	{
		return Scripture::RenderText(
			renderer,
			rect,
			Scripture::FONT_KEY_DIALOG,
			15,
			literal,
			Constants::ButtonTextColor
		);
	}

	SDL_Texture* SectionTextureInternal(const char* literal, SDL_FRect* rect, SDL_Renderer* renderer)
	{
		return Scripture::RenderText(
			renderer,
			rect,
			Scripture::FONT_KEY_DIALOG,
			28,
			literal,
			Constants::ButtonTextColor,
			true
		);
	}

	SDL_Texture* ChapterTextureInternal(const char* literal, SDL_FRect* rect, SDL_Renderer* renderer)
	{
		return Scripture::RenderText(
			renderer,
			rect,
			Scripture::FONT_KEY_DIALOG,
			17,
			literal,
			Constants::ChapterTextColor,
			true
		);
	}

	std::shared_ptr<CreditsMention> AddMentionInternal(SDL_Texture* mentionTexture, const SDL_FRect* textureRect)
	{
		auto mention = std::make_shared<CreditsMention>();
		mention->rect = *textureRect;
		mention->texture = mentionTexture;
		creditMentions.push_back(mention);

		return mention;
	}

	void AddSectionInternal(SDL_Texture* sectionTexture, const SDL_FRect* textureRect)
	{
		CreditsSection section;
		section.rect = *textureRect;
		section.texture = sectionTexture;
		creditSections.emplace_back(section);
	}

	std::shared_ptr<CreditsChapter> AddChapterInternal(SDL_Texture* chapterTexture, const SDL_FRect* textureRect)
	{
		auto chapter = std::make_shared<CreditsChapter>();
		chapter->rect = *textureRect;
		chapter->texture = chapterTexture;
		creditChapters.push_back(chapter);

		return chapter;
	}

	void TeardownInternal()
	{
		Drawing::DestroyTexture(&backTexture);

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