#include "PlayerMomentum.h"
#include "..\src\dings\dings.h"

#pragma once

class Sprite : public BlockObject
{
public:
	Sprite::Sprite();
	Sprite::Sprite(const Sprite& obj) { };
	Sprite& operator=(const Sprite& obj) { return *this; };
	Sprite::Sprite(Sprite&& obj) { };
	virtual ~Sprite();

	void InitializeIn(Dings::DingIDs dingID, Platform::String^ instanceName, std::shared_ptr<Level> inLevel, unsigned positionInLevelX, unsigned positionInLevelY, Facings mobFacing);
	void PushX(float accelerationRate, float attenuationRate, float gripFactor, float mediumViscosity);
	void PushY(float accelerationRate, float attenuationRate, float gripFactor, float mediumViscosity);
	std::shared_ptr<BlockObject> Update();

	Platform::String^ Name;	
	PlayerMomentum Momentum;
	Facings Facing;

	OrientabilityIndexRotatory m_Orientation;
};