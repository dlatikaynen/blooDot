#include "pch.h"
#include "playerstate.h"

#define MAX_VITAL_VALUE ((char)0xf)

namespace blooDot::Player
{
	int NumPlayers = 0;
	MobState Player[4] = {};
	std::list<MobState> Mobs;

	MobState* GetState(int ofPlayerIndex)
	{
		return &Player[ofPlayerIndex];
	}

	void VitalIncrease(MobState* mobState, VitalSlotDedication dedication)
	{
		Uint16 ffp2 = 0b0000000000001111 << (4 * dedication);
		auto currentValue = static_cast<char>((mobState->Vitals & ffp2) >> (4 * dedication));
		if (currentValue < MAX_VITAL_VALUE)
		{
			++currentValue;
			mobState->Vitals = (mobState->Vitals & ~ffp2) + (currentValue << (4 * dedication));
		}
	}

	void VitalDecrease(MobState* mobState, VitalSlotDedication dedication)
	{
		Uint16 ffp2 = 0b0000000000001111 << (4 * dedication);
		auto currentValue = static_cast<char>((mobState->Vitals & ffp2) >> (4 * dedication));
		if (currentValue > 0)
		{
			--currentValue;
			mobState->Vitals = (mobState->Vitals & ~ffp2) + (currentValue << (4 * dedication));
		}
	}
}
