#include "..\..\dings\dings.h"
#include "PlayerMomentum.h"

const float PlayerMomentum::speedCapX = blooDot::Consts::SQUARE_WIDTH / 2.2;
const float PlayerMomentum::speedCapY = blooDot::Consts::SQUARE_WIDTH / 2.2;

void PlayerMomentum::HitTheWall(Facings directionFacing)
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