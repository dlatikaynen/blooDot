#pragma once
#include <SDL.h>
#include <iostream>

namespace blooDot::Savegame
{
	void Save(int savegameIndex, bool isAutosave = false);
	void Load(int savegameIndex, int savepointIndex);
}