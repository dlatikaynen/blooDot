#include "pch.h"
#include "settings.h"
#include "enums.h"
#include "xlations.h"
#include <stdio.h>
#include "constants.h"
#include "chunk-constants.h"
#include "dexassy.h"

SettingsStruct Settings;

namespace blooDot::Settings
{
	void Load()
	{
		const auto settingsFile = SDL_RWFromFile(SettingsFileName, "rb");
		if (!settingsFile)
		{
			const auto loadError = SDL_GetError();
			ReportError("Could not load settings, fallback to defaults", loadError);
			Default();
			Save();
			return;
		}

		settingsFile->read(settingsFile, &::Settings, sizeof(SettingsStruct), 1);
		const SettingsStruct validator;
		const auto isValid = _memicmp(
			(const void*)(&validator.Preamble0),
			(const void*)(&::Settings.Preamble0),
			sizeof(char) * 8
		);

		if (isValid != 0)
		{
			ReportError("Could not load settings, fallback to defaults", "Settings store corrupted");
			Default();
			Save();
			return;
		}

		settingsFile->close(settingsFile);
	}

	void PreloadControllerMappings()
	{
		SDL_RWops* mappingsFile;
		const auto mappingsMem = Retrieve(CHUNK_KEY_GAME_CONTROLLER_DB_TXT, &mappingsFile);
		if (SDL_GameControllerAddMappingsFromRW(mappingsFile, 0) == -1)
		{
			const auto loadError = SDL_GetError();
			ReportError("Could not load controller mappings", loadError);
		}

		mappingsFile->close(mappingsFile);
		SDL_free(mappingsMem);
	}

	void ApplyLanguage()
	{
		switch (::Settings.SettingUserInterfaceLanguage)
		{
		case UserInterfaceLanguages::UIL_GERMAN:
			blooDot::de::Set();
			return;

		case UserInterfaceLanguages::UIL_FINNISH:
			blooDot::fi::Set();
			return;

		case UserInterfaceLanguages::UIL_UKRAINIAN:
			blooDot::ua::Set();
			return;
		}

		blooDot::en::Set();
	}

	void Save()
	{
		const auto settingsFile = SDL_RWFromFile(SettingsFileName, "wb");
		if (!settingsFile)
		{
			const auto openError = SDL_GetError();
			ReportError("Could not open settings file for writing", openError);
			return;
		}

		const auto numWritten = settingsFile->write(
			settingsFile,
			(void*)(&::Settings),
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

	void Default()
	{
		::Settings.SettingViewportResolution = VR_TEMPLE;
	}

	int GetDisplayIndexCapped()
	{
		auto storedIndex = ::Settings.FullscreenDisplayIndex;
		auto numDisplays = SDL_GetNumVideoDisplays();
		int cappedIndex = 0;
		if (numDisplays < 0)
		{
			const auto queryError = SDL_GetError();
			ReportError("Could not query number of video displays", queryError);
			numDisplays = 0;
		}

		if (storedIndex < numDisplays)
		{
			cappedIndex = storedIndex;
		}

		return cappedIndex;
	}

	int GetLogicalArenaWidth()
	{
		switch (::Settings.SettingViewportResolution)
		{
		case ViewportResolutions::VR_MODEX:
			return 320;

		case ViewportResolutions::VR_HERCMONO:
			return 720;

		case ViewportResolutions::VR_SVGA:
			return 800;

		case ViewportResolutions::VR_NOTEBOOK:
			return 1280;

		case ViewportResolutions::VR_SQUARE:
		{
			SDL_Rect rect = { 0 };
			if (_GetScreenDimensions(Settings::GetDisplayIndexCapped(), &rect))
			{
				return std::min(rect.h, rect.w);
			}

			return GodsPreferredHight;
		}

		case ViewportResolutions::VR_MAXOUT:
			return GetPhysicalArenaWidth();

		default:
			return GodsPreferredHight;
		}
	}

	int GetPhysicalArenaWidth()
	{
		switch (::Settings.SettingViewportResolution)
		{
		case ViewportResolutions::VR_SQUARE:
		case ViewportResolutions::VR_MAXOUT:
		{
			SDL_Rect rect = { 0 };
			if (_GetScreenDimensions(Settings::GetDisplayIndexCapped(), &rect))
			{
				return rect.w;
			}

			return GodsPreferredWidth;
		}

		default:
			return GetLogicalArenaWidth();
		}
	}

	int GetLogicalArenaHeight()
	{
		switch (::Settings.SettingViewportResolution)
		{
		case ViewportResolutions::VR_MODEX:
			return 240;

		case ViewportResolutions::VR_HERCMONO:
			return 348;

		case ViewportResolutions::VR_SVGA:
			return 600;

		case ViewportResolutions::VR_NOTEBOOK:
			return 720;

		case ViewportResolutions::VR_SQUARE:
		{
			SDL_Rect rect = { 0 };
			if (_GetScreenDimensions(Settings::GetDisplayIndexCapped(), &rect))
			{
				return std::min(rect.w, rect.h);
			}

			return GodsPreferredHight;
		}

		case ViewportResolutions::VR_MAXOUT:
			return GetPhysicalArenaHeight();

		default:
			return GodsPreferredHight;
		}
	}

	int GetPhysicalArenaHeight()
	{
		switch (::Settings.SettingViewportResolution)
		{
		case ViewportResolutions::VR_SQUARE:
		case ViewportResolutions::VR_MAXOUT:
		{
			SDL_Rect rect = { 0 };
			if (_GetScreenDimensions(Settings::GetDisplayIndexCapped(), &rect))
			{
				return rect.h;
			}

			return GodsPreferredHight;
		}

		default:
			return GetLogicalArenaHeight();
		}
	}

	bool ShowMinimap()
	{
		return ::Settings.ShowMinimap != 0;
	}

	bool _GetScreenDimensions(int displayIndex, _Inout_ SDL_Rect* dimensions)
	{
		auto& rect = (*dimensions);
		SDL_DisplayMode displayMode = {};
		if (SDL_GetDesktopDisplayMode(displayIndex, &displayMode) < 0)
		{
			const auto& queryError = SDL_GetError();
			ReportError("Could not query screen dimensions", queryError);
			return false;
		}

		rect.w = displayMode.w;
		rect.h = displayMode.h;

		return true;
	}
}