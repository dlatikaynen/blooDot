#include "pch.h"
#include "settings.h"
#include "enums.h"

#include <stdio.h>

SettingsStruct Settings;

void LoadSettings()
{	
	const auto settingsFile = SDL_RWFromFile(settingsFileName, "rb");
	if (!settingsFile)
	{
		const auto loadError = SDL_GetError();
		ReportError("Could not load settings, fallback to defaults", loadError);
		DefaultSettings();
		SaveSettings();
		return;
	}

	settingsFile->read(settingsFile, &Settings, sizeof(SettingsStruct), 1);
	const SettingsStruct validator;
	const auto isValid = _memicmp(
		(const void*)(&validator.Preamble0),
		(const void*)(&Settings.Preamble0),
		sizeof(char) * 8
	);

	if (isValid != 0)
	{
		ReportError("Could not load settings, fallback to defaults", "Settings store corrupted");
		DefaultSettings();
		SaveSettings();
		return;
	}

	settingsFile->close(settingsFile);
}

void SaveSettings()
{
	const auto settingsFile = SDL_RWFromFile(settingsFileName, "wb");
	if (!settingsFile)
	{
		const auto loadError = SDL_GetError();
		ReportError("Could not open settings file for writing", loadError);
		return;
	}

	const auto numWritten = settingsFile->write(
		settingsFile,
		(void *)(&Settings),
		sizeof(SettingsStruct),
		1
	);

	if (numWritten != 1)
	{
		const auto writeError = SDL_GetError();
		ReportError("Failed to write settings", writeError);
	}

	settingsFile->close(settingsFile);
}

void DefaultSettings()
{
	Settings.SettingViewportResolution = VR_TEMPLE;
}