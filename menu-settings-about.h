#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>
#include "splash.h"
#include "enums.h"

extern SDL_Texture* BeginRenderDrawing(SDL_Renderer* renderTarget, const int canvasWidth, const int canvasHeight);

namespace blooDot::MenuSettingsAbout
{
	typedef struct CreditsMentionStruct {
		SDL_Texture* texture;
		SDL_Rect rect;
	} CreditsMention;

	typedef struct CreditsChapterStruct {
		SDL_Texture* texture;
		SDL_Rect rect;
		std::vector<std::shared_ptr<CreditsMention>> mentions;
	} CreditsChapter;

	typedef struct CreditsSectionStruct {
		SDL_Texture* texture;
		SDL_Rect rect;
		std::vector<std::shared_ptr<CreditsChapter>> chapters;
	} CreditsSection;

	bool MenuLoop(SDL_Renderer*);

	void _PrepareText(SDL_Renderer* renderer);
	SDL_Texture* _MentionTexture(const char* literal, SDL_Rect* rect, SDL_Renderer* renderer);
	SDL_Texture* _SectionTexture(const char* literal, SDL_Rect* rect, SDL_Renderer* renderer);
	SDL_Texture* _ChapterTexture(const char* literal, SDL_Rect* rect, SDL_Renderer* renderer);
	void _AddSection(SDL_Texture* sectionTexture, SDL_Rect* textureRect);
	std::shared_ptr<CreditsChapter> _AddChapter(SDL_Texture* chapterTexture, SDL_Rect* textureRect);
	std::shared_ptr<CreditsMention> _AddMention(SDL_Texture* mentionTexture, SDL_Rect* textureRect);
	void _Teardown();
}