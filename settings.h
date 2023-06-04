#pragma once
#include <SDL.h>
#include "enums.h"

extern void ReportError(const char*, const char*);

namespace blooDot::Settings
{
	void Load();
	void PreloadControllerMappings();
	void ApplyLanguage();
	void Save();
	void Default();

	int GetDisplayIndex();
	int GetLogicalArenaWidth();
	int GetPhysicalArenaWidth();
	int GetLogicalArenaHeight();
	int GetPhysicalArenaHeight();
	bool ShowMinimap();

	bool _GetScreenDimensions(_Inout_ SDL_Rect* dimensions);
}

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
	ViewportResolutions SettingViewportResolution = VR_TEMPLE;
	UserInterfaceLanguages SettingUserInterfaceLanguage = UIL_AMERICAN;
	unsigned short CurrentSavegameIndex = 0;  // those are 1-based and appear in the file name
	unsigned short OccupiedSavegameSlots = 0; // this is a bitmask with the LSB corresponding to savegameindex 1
	char ShowMinimap = 0x1;
	unsigned char FullscreenDisplayIndex = 0;
} SettingsStruct;
