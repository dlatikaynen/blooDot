#include "..\..\PreCompiledHeaders.h"

#pragma once

class PlayerMomentum
{
public:
	static const float speedCapX;
	static const float speedCapY;

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
