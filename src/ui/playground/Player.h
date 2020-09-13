#include "PlayerMomentum.h"
#include "..\src\dings\dings.h"

#pragma once

class Player : public BlockObject
{
public:
	Player(const Player& obj) { };
	Player& operator=(const Player& obj) { return *this; };
	Player();
	Player(Player&& obj) { };
	virtual ~Player();

	void InitializeIn(Platform::String^ playerName, std::shared_ptr<Level> inLevel, unsigned positionInLevelX, unsigned positionInLevelY, Facings mobFacing);
	void PushX(float accelerationRate, float attenuationRate, float gripFactor, float mediumViscosity);
	void PushY(float accelerationRate, float attenuationRate, float gripFactor, float mediumViscosity);
	std::shared_ptr<BlockObject> Update();

	Platform::String^ Name;	
	PlayerMomentum Momentum;
	Facings Facing;

	OrientabilityIndexRotatory m_Orientation;
};