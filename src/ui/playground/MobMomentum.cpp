#include "..\..\dings\dings.h"
#include "MobMomentum.h"

const float MobMomentum::speedCapX = blooDot::Consts::SQUARE_WIDTH / 2.2f;
const float MobMomentum::speedCapY = blooDot::Consts::SQUARE_WIDTH / 2.2f;

void MobMomentum::HitTheWall(Facings directionFacing)
{
	switch (directionFacing)
	{
	case Facings::East:
	case Facings::West:
		this->accelerationX = 0;
		this->attenuationX = 0;
		this->speedX = 0;
		break;

	case Facings::North:
	case Facings::South:
		this->accelerationY = 0;
		this->attenuationY = 0;
		this->speedY = 0;
		break;
	}
}