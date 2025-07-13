#ifndef SETTINGS_H
#define SETTINGS_H

#include <SDL3/SDL.h>
#include "../../src/shared-constants.h"

namespace blooDot::Settings {
    void Load();
    void PreloadControllerMappings();
    void ApplyLanguage();
    void Save();
    void Default();

    int GetDisplayIndexCapped();
    int GetLogicalArenaWidth();
    int GetPhysicalArenaWidth();
    int GetLogicalArenaHeight();
    int GetPhysicalArenaHeight();
    bool ShowMinimap();

    bool GetScreenDimensionsInternal(int displayIndex, SDL_FRect* dimensions);

    typedef struct SettingsStruct
    {
        char Preamble0 = '\4';
        char Preamble1 = 'L';
        char Preamble2 = 'S';
        char Preamble3 = 'L';
        char Preamble4 = '\3';
        char Preamble5 = 'J';
        char Preamble6 = 'M';
        char Preamble7 = 'L';
        char Preamble8 = '\5';
        char Preamble9 = '\0';
        Constants::ViewportResolutions SettingViewportResolution = Constants::VR_TEMPLE;
        Constants::UserInterfaceLanguages SettingUserInterfaceLanguage = Constants::UIL_AMERICAN;
        unsigned short CurrentSavegameIndex = 0;  // those are 1-based and appear in the file name
        unsigned short OccupiedSavegameSlots = 0; // this is a bitmask with the LSB corresponding to savegameindex 1
        char ShowMinimap = 0x1;
        unsigned char FullscreenDisplayIndex = 0;
    } SettingsStruct;

    inline SettingsStruct SettingsData;
}
#endif //SETTINGS_H
