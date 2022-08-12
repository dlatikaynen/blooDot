#pragma once
#include <SDL.h>
#include "enums.h"

constexpr char const* settingsFileName = "blooDot.ligma";

extern void ReportError(const char*, const char*);

void LoadSettings();
void SaveSettings();
void DefaultSettings();

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
} SettingsStruct;
