#include "pch.h"
#include "playerstate.h"

namespace blooDot::Player
{
	int NumPlayers = 0;
	MobState Player[4] = {};
	std::list<MobState> Mobs;

	MobState* GetState(int ofPlayerIndex)
	{
		return &Player[ofPlayerIndex];
	}
}