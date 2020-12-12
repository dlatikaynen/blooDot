#include "..\..\PreCompiledHeaders.h"

#pragma once

class MobMomentum
{
public:
	//static const float speedCapX;
	//static const float speedCapY;
	const static float speedCapX;
	const static float speedCapY;

	void HitTheWall(Facings directionFacing);

	/* logical pixels per frame */
	float speedX; //vectorial
	float speedY; //vectorial
	float accelerationX; //vectorial
	float accelerationY; //vectorial
	float attenuationX; //absolute
	float attenuationY; //absolute
	
	/* always apply in both dimensions equally */
	float gripFactor; //absolute
	float mediumViscosity; //absolute
};
