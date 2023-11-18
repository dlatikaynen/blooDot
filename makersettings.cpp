#include "pch.h"
#include "makersettings.h"
#include "enums.h"
#include "xlations.h"
#include <stdio.h>
#include "constants.h"

namespace blooDot::MakerSettings
{
	MakerSettingsStruct MakerSettings;
	bool isLoaded = false;

	void Load()
	{
		const auto makerSettingsFile = SDL_RWFromFile(MakerSettingsFileName, "rb");
		if (!makerSettingsFile)
		{
			const auto loadError = SDL_GetError();
			ReportError("Could not load maker settings, fallback to defaults", loadError);
			Default();
			Save();

			return;
		}

		makerSettingsFile->read(makerSettingsFile, &MakerSettings, sizeof(MakerSettingsStruct), 1);
		const MakerSettingsStruct validator;
		const auto isValid = _memicmp(
			(const void*)(&validator.Preamble0),
			(const void*)(&MakerSettings.Preamble0),
			sizeof(char) * 8
		);

		if (isValid != 0)
		{
			ReportError("Could not load maker settings, fallback to defaults", "Maker settings store corrupted");
			Default();
			Save();

			return;
		}

		makerSettingsFile->close(makerSettingsFile);
	}

	void Save()
	{
		const auto makerSettingsFile = SDL_RWFromFile(MakerSettingsFileName, "wb");
		if (!makerSettingsFile)
		{
			const auto openError = SDL_GetError();
			ReportError("Could not open maker settings file for writing", openError);
			return;
		}

		const auto numWritten = makerSettingsFile->write(
			makerSettingsFile,
			(void*)(&MakerSettings),
			sizeof(MakerSettingsStruct),
			1
		);

		if (numWritten != 1)
		{
			const auto writeError = SDL_GetError();
			ReportError("Failed to write maker settings", writeError);
		}

		makerSettingsFile->close(makerSettingsFile);
	}

	void Default()
	{
		MakerSettings.HasLastLocation = 0x0;
	}

	bool HasLastLocation()
	{
		_EnsureLoaded();

		return MakerSettings.HasLastLocation == 0x1;
	}

	char GetNumberOfTeleportTargets()
	{
		return MakerSettings.NumberOfTeleportTargets;
	}

	void _EnsureLoaded()
	{
		if (isLoaded)
		{
			return;
		}

		Load();
	}
}