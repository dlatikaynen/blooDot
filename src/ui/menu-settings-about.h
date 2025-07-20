#ifndef MENU_SETTINGS_ABOUT_H
#define MENU_SETTINGS_ABOUT_H
#include <memory>
#include <vector>
#include <SDL3/SDL.h>

namespace blooDot::MenuSettingsAbout
{
    typedef struct CreditsMentionStruct {
        SDL_Texture* texture;
        SDL_FRect rect;
    } CreditsMention;

    typedef struct CreditsChapterStruct {
        SDL_Texture* texture;
        SDL_FRect rect;
        std::vector<std::shared_ptr<CreditsMention>> mentions;
    } CreditsChapter;

    typedef struct CreditsSectionStruct {
        SDL_Texture* texture = nullptr;
        SDL_FRect rect = {};
        std::vector<std::shared_ptr<CreditsChapter>> chapters;
    } CreditsSection;

    bool MenuLoop(SDL_Renderer*);
    void PrepareTextInternal(SDL_Renderer* renderer);
    SDL_Texture* MentionTextureInternal(const char* literal, SDL_FRect* rect, SDL_Renderer* renderer);
    SDL_Texture* SectionTextureInternal(const char* literal, SDL_FRect* rect, SDL_Renderer* renderer);
    SDL_Texture* ChapterTextureInternal(const char* literal, SDL_FRect* rect, SDL_Renderer* renderer);
    void AddSectionInternal(SDL_Texture* sectionTexture, const SDL_FRect* textureRect);
    std::shared_ptr<CreditsChapter> AddChapterInternal(SDL_Texture* chapterTexture, const SDL_FRect* textureRect);
    std::shared_ptr<CreditsMention> AddMentionInternal(SDL_Texture* mentionTexture, const SDL_FRect* textureRect);
    void TeardownInternal();
}
#endif //MENU_SETTINGS_ABOUT_H
