#pragma once
#include "pch.h"
#include "savegame.h"

namespace blooDot::Savegame
{
	void Save(int savegameIndex, bool isAutosave)
	{
		std::cout << savegameIndex << ", " << isAutosave;
	}

	void Load(int savegameIndex, int savepointIndex)
	{
		std::cout << savegameIndex << ", " << savepointIndex;
	}
}