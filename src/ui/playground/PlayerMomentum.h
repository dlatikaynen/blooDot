#include "..\..\PreCompiledHeaders.h"

#pragma once

class PlayerMomentum
{
public:
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