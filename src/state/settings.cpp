#include "settings.h"
#include <algorithm>

#include "../../main.h"
#include "../../src/shared-constants.h"
#include "../../res/xlations.h"
#include "../../res/chunk-constants.h"
#include "../../xassy/dexassy.h"

namespace blooDot::Settings
{
	void Load()
	{
		const auto settingsFile = SDL_IOFromFile(Constants::SettingsFileName, "rb");

		if (!settingsFile)
		{
			const auto loadError = SDL_GetError();

			ReportError("Could not load settings, fallback to defaults", loadError);
			Default();
			Save();

			return;
		}

		SDL_ReadIO(settingsFile, &SettingsData, sizeof(SettingsStruct));
		const SettingsStruct validator;
		const auto isValid = _memicmp(
			&validator.Preamble0,
			&SettingsData.Preamble0,
			sizeof(char) * 8
		);

		if (isValid != 0)
		{
			ReportError("Could not load settings, fallback to defaults", "Settings store corrupted");
			Default();
			Save();

			return;
		}

		SDL_CloseIO(settingsFile);
	}

	void PreloadControllerMappings()
	{
		SDL_IOStream* mappingsFile;
		const auto mappingsMem = Retrieve(CHUNK_KEY_GAME_CONTROLLER_DB_TXT, &mappingsFile);

		if (SDL_AddGamepadMappingsFromIO(mappingsFile, false) == -1)
		{
			const auto loadError = SDL_GetError();

			ReportError("Could not load controller mappings", loadError);
		}

		SDL_CloseIO(mappingsFile);
		SDL_free(mappingsMem);
	}

	void ApplyLanguage()
	{
		switch (SettingsData.SettingUserInterfaceLanguage)
		{
		case Constants::UserInterfaceLanguages::UIL_GERMAN:
			de::Set();
			return;

		case Constants::UserInterfaceLanguages::UIL_FINNISH:
			fi::Set();
			return;

		case Constants::UserInterfaceLanguages::UIL_UKRAINIAN:
			ua::Set();
			return;

		default:
			break;
		}

		en::Set();
	}

	void Save()
	{
		const auto settingsFile = SDL_IOFromFile(Constants::SettingsFileName, "wb");

		if (!settingsFile)
		{
			const auto openError = SDL_GetError();

			ReportError("Could not open settings file for writing", openError);

			return;
		}

		const auto numWritten = SDL_WriteIO(
			settingsFile,
			&SettingsData,
			sizeof(SettingsStruct)
		);

		if (numWritten != 1)
		{
			const auto writeError = SDL_GetError();

			ReportError("Failed to write settings", writeError);
		}

		SDL_CloseIO(settingsFile);
	}

	void Default()
	{
		SettingsData.SettingViewportResolution = Constants::VR_TEMPLE;
	}

	int GetDisplayIndexCapped()
	{
		const auto storedIndex = SettingsData.FullscreenDisplayIndex;
		int numDisplays;

		if (const auto displays = SDL_GetDisplays(&numDisplays); displays == nullptr) {
			const auto queryError = SDL_GetError();

			ReportError("Could not query number of video displays", queryError);
			numDisplays = 0;
		} else {
			SDL_free(displays);
		}

		int cappedIndex = 0;

		if (storedIndex < numDisplays)
		{
			cappedIndex = storedIndex;
		}

		return cappedIndex;
	}

	int GetLogicalArenaWidth()
	{
		switch (SettingsData.SettingViewportResolution)
		{
		case Constants::ViewportResolutions::VR_MODEX:
			return 320;

		case Constants::ViewportResolutions::VR_HERCMONO:
			return 720;

		case Constants::ViewportResolutions::VR_SVGA:
			return 800;

		case Constants::ViewportResolutions::VR_NOTEBOOK:
			return 1280;

		case Constants::ViewportResolutions::VR_SQUARE:
		{
			SDL_FRect rect = {};
			if (GetScreenDimensionsInternal(GetDisplayIndexCapped(), &rect))
			{
				return static_cast<int>(std::min(rect.h, rect.w));
			}

			return Constants::GodsPreferredHight;
		}

		case Constants::ViewportResolutions::VR_MAXOUT:
			return GetPhysicalArenaWidth();

		default:
			return Constants::GodsPreferredHight;
		}
	}

	int GetPhysicalArenaWidth()
	{
		switch (SettingsData.SettingViewportResolution)
		{
		case Constants::ViewportResolutions::VR_SQUARE:
		case Constants::ViewportResolutions::VR_MAXOUT:
		{
			SDL_FRect rect = {};
			if (GetScreenDimensionsInternal(GetDisplayIndexCapped(), &rect))
			{
				return static_cast<int>(rect.w);
			}

			return Constants::GodsPreferredWidth;
		}

		default:
			return GetLogicalArenaWidth();
		}
	}

	int GetLogicalArenaHeight()
	{
		switch (SettingsData.SettingViewportResolution)
		{
		case Constants::ViewportResolutions::VR_MODEX:
			return 240;

		case Constants::ViewportResolutions::VR_HERCMONO:
			return 348;

		case Constants::ViewportResolutions::VR_SVGA:
			return 600;

		case Constants::ViewportResolutions::VR_NOTEBOOK:
			return 720;

		case Constants::ViewportResolutions::VR_SQUARE:
		{
			SDL_FRect rect = {};

			if (GetScreenDimensionsInternal(GetDisplayIndexCapped(), &rect))
			{
				return static_cast<int>(std::min(rect.w, rect.h));
			}

			return Constants::GodsPreferredHight;
		}

		case Constants::ViewportResolutions::VR_MAXOUT:
			return GetPhysicalArenaHeight();

		default:
			return Constants::GodsPreferredHight;
		}
	}

	int GetPhysicalArenaHeight()
	{
		switch (SettingsData.SettingViewportResolution)
		{
		case Constants::ViewportResolutions::VR_SQUARE:
		case Constants::ViewportResolutions::VR_MAXOUT:
		{
			SDL_FRect rect = {};

			if (GetScreenDimensionsInternal(GetDisplayIndexCapped(), &rect))
			{
				return static_cast<int>(rect.h);
			}

			return Constants::GodsPreferredHight;
		}

		default:
			return GetLogicalArenaHeight();
		}
	}

	bool ShowMinimap()
	{
		return SettingsData.ShowMinimap != 0;
	}

	bool GetScreenDimensionsInternal(const int displayIndex, SDL_FRect* dimensions)
	{
		auto& rect = (*dimensions);
		const auto displayMode = SDL_GetDesktopDisplayMode(displayIndex);

		if (displayMode == nullptr)
		{
			const auto& queryError = SDL_GetError();
			ReportError("Could not query screen dimensions", queryError);
			return false;
		}

		rect.w = static_cast<float>(displayMode->w);
		rect.h = static_cast<float>(displayMode->h);

		return true;
	}
}