#ifndef CPUCHAN_H
#define CPUCHAN_H
#include <string>
#include <SDL3/SDL.h>

namespace blooDot::Ui::GpuChan {
    bool GpuChanLoop(SDL_Renderer* renderer, const char* message, const char* title, const std::string& bubble);

    void EnterInternal(SDL_Renderer* renderer);
    void PullOutInternal();
}
#endif //CPUCHAN_H
