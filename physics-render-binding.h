#pragma once
#include <box2d/box2d.h>
#include "flap.h"

extern FlapAwareness GetFlapAwareness(int flapIndex);

namespace blooDot::Physics
{
	void AttachWorldPhysics(b2World* world);
}