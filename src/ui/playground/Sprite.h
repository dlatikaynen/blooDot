#include "MobMomentum.h"
#include "src\dings\dings.h"

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
	std::shared_ptr<BlockObject> Update(float timeTotal, float timeDelta);

	bool TimingRotationPending();
	bool TimingRotationClear();

	void PushTheWall(std::shared_ptr<BlockObject> pushableObject, Facings towardsDirection);

	Platform::String^ Name;	
	MobMomentum Momentum;
	Facings Facing;

private:
	float	m_timeAccrued;
	bool	m_timeForRotation;
};