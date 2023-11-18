#pragma once
#include <SDL2/SDL.h>
#include "enums.h"

extern void ReportError(const char*, const char*);

namespace blooDot::MakerSettings
{
	void Load();
	void Save();
	void Default();

	bool HasLastLocation();
	char GetNumberOfTeleportTargets();

	void _EnsureLoaded();
}

typedef struct MakerSettingsStruct
{
	char Preamble0 = '\3';
	char Preamble1 = 'L';
	char Preamble2 = 'S';
	char Preamble3 = 'L';
	char Preamble4 = '\5';
	char Preamble5 = 'J';
	char Preamble6 = 'M';
	char Preamble7 = 'L';
	char Preamble8 = '\3';
	char Preamble9 = '\0';
	char HasLastLocation = 0x0;
	char NumberOfTeleportTargets = 0x0;
} MakerSettingsStruct;
