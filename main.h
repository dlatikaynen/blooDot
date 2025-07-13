#ifndef MAIN_H
#define MAIN_H
#include "SDL3/SDL.h"

namespace blooDot {
    constexpr auto NotG = "blooDot";

    constexpr int RETVAL_OK = 0;
    constexpr int RETVAL_SDL_INIT_FAIL = 0xacab01;
    constexpr int RETVAL_SDL_NO_RENDER_DRIVERS_FAIL = 0xacab02;
    constexpr int RETVAL_DEXASSY_FAIL = 0xacab04;
    constexpr int RETVAL_CREATE_MAIN_UI_FAIL = 0xacab05;
    constexpr int RETVAL_AUDIO_INIT_FAIL = 0xacab06;
    constexpr int RETVAL_LOAD_FONTS_FAIL = 0xacab07;
    // next: 08
    constexpr int RETVAL_NO_CLEAN_SHUTDOWN = 0xacab03;

    inline SDL_Window* mainWindow = nullptr;
    inline SDL_Renderer* renderer = nullptr;
    inline bool Quit = false;

    void ReportError(const char* message, const char* error);
    int xassy();
    bool CreateMainUiWindow();
    void CleanupMainUiWindow();
}
#endif //MAIN_H
