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

	void VitalIncrease(MobState* mobState, VitalSlotDedication dedication)
	{
		Uint16 ffp2 = 0b0000000000001111 << (4 * dedication);
		Uint16 capaMask = 0b0000000000000111 << (3 * dedication);
		Uint16 dediMask = 0b0000000000000011 << (2 * dedication);
		auto capacity = (mobState->Capacity & capaMask) >> (3 * dedication);
		auto currentValue = static_cast<char>((mobState->Vitals & ffp2) >> (4 * dedication));
		if (currentValue < capacity)
		{
			++currentValue;
			mobState->Vitals = (mobState->Vitals & ~ffp2) + (currentValue << (4 * dedication));
			if (currentValue == 1)
			{
				/*
				* do we refill a previously repurposed or empty
				* petal? then we will simply reinizialize the petal
				* --> assign to it its native dedication
				*/
				mobState->SlotDedication = static_cast<Uint8>((mobState->SlotDedication & ~dediMask) + (dedication << (2 * dedication)));
			}
			else
			{

				/* do we spill over?
				 * (in which case we kill the spilled-into petal
				 * unless we're already back around */
			}
		}
	}

	/// <summary>
	/// Decrease cannot alter slot dedication.
	/// An empty slot is only ever refilled if the mob
	/// picks up one of its original dedication
	/// </summary>
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
