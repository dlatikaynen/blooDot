#ifndef SFX_H
#define SFX_H

#include <SDL3_mixer/SDL_mixer.h>

namespace blooDot::Sfx
{
    enum SoundEffect {
        SFX_BULLET_DECAY,
        SFX_BUMP,
        SFX_SELCHG,
        SFX_SELCONF,
        SFX_ASTERISK,
        SFX_CRICKET_FRITZ,
        SFX_CRICKET_FRANZ,
        SFX_FLOOR,
        SFX_WALLS,
        SFX_ROOOF
    };

    void PreloadMenuSfx();
    void PreloadGameSfx();
    void Play(SoundEffect effect);
    void Teardown();

    void PreloadSingleInternal(const SoundEffect& effect);
    Mix_Chunk* LoadSingleInternal(const SoundEffect& effect);
    int GetResourceKeyInternal(const SoundEffect& effect);
}

#endif //SFX_H
