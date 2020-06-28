#include "PlayerMomentum.h"
#include "..\src\dings\dings.h"

#pragma once

class Player : public Object
{
public:
	Player(const Player& obj) { };
	Player& operator=(const Player& obj) { return *this; };
	Player();
	Player(Player&& obj) { };
	virtual ~Player();

	void PushX(float accelerationRate, float attenuationRate, float gripFactor, float mediumViscosity);
	void PushY(float accelerationRate, float attenuationRate, float gripFactor, float mediumViscosity);
	void Update();

	Platform::String^ Name;	
	PlayerMomentum Momentum;
	Facings Facing;
};
