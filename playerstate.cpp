#include "pch.h"
#include "playerstate.h"

namespace blooDot::Player
{
	auto NumPlayers = 0;
	MobState Player[4] = {};
	std::list<MobState> Mobs;
}