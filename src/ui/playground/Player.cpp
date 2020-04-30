#include "..\src\world\Object.h"
#include "..\src\world\Level.h"

#include "Player.h"

Player::Player()
{
	this->Momentum.speedX = 0.0f;
	this->Momentum.speedY = 0.0f;
	this->Momentum.accelerationX = 0.0f;
	this->Momentum.accelerationY = 0.0f;
	this->Momentum.attenuationX = 0.0f;
	this->Momentum.attenuationY = 0.0f;	
}

Player::~Player()
{	
}

void Player::PushX(float accelerationRate, float attenuationRate, float gripFactor, float mediumViscosity)
{
	this->Momentum.accelerationX = accelerationRate;
	this->Momentum.attenuationX = attenuationRate;
	this->Momentum.gripFactor = gripFactor;
	this->Momentum.mediumViscosity = mediumViscosity;
}

void Player::PushY(float accelerationRate, float attenuationRate, float gripFactor, float mediumViscosity)
{
	this->Momentum.accelerationY = accelerationRate;
	this->Momentum.attenuationY = attenuationRate;
	this->Momentum.gripFactor = gripFactor;
	this->Momentum.mediumViscosity = mediumViscosity;
}

void Player::Update()
{
	D2D1_RECT_F myBoundingBox;

	/* accelerate with momentary acceleration */
	this->Momentum.speedX += this->Momentum.accelerationX;
	this->Momentum.speedY += this->Momentum.accelerationY;

	/* attenuate speed by environment viscosity */
	if (this->Momentum.mediumViscosity > 0.0f)
	{
		if (this->Momentum.speedX > 0.0f)
		{
			this->Momentum.speedX = max(this->Momentum.speedX - this->Momentum.mediumViscosity, 0.0f);
		}
		else if (this->Momentum.speedX < 0.0f)
		{
			this->Momentum.speedX = min(this->Momentum.speedX + this->Momentum.mediumViscosity, 0.0f);
		}

		if (this->Momentum.speedY > 0.0f)
		{
			this->Momentum.speedY = max(this->Momentum.speedY - this->Momentum.mediumViscosity, 0.0f);
		}
		else if (this->Momentum.speedY < 0.0f)
		{
			this->Momentum.speedY = min(this->Momentum.speedY + this->Momentum.mediumViscosity, 0.0f);
		}
	}

	/* attenuate acceleration, if any left */	
	if (this->Momentum.attenuationX > 0.0f)
	{
		if (this->Momentum.accelerationX > 0.0f)
		{
			this->Momentum.accelerationX = max(this->Momentum.accelerationX - this->Momentum.attenuationX, 0.0f);
		}
		else if(this->Momentum.accelerationX<0.0f)
		{
			this->Momentum.accelerationX = min(this->Momentum.accelerationX + this->Momentum.attenuationX, 0.0f);
		}
	}

	if (this->Momentum.attenuationY > 0.0f)
	{
		if (this->Momentum.accelerationY > 0.0f)
		{
			this->Momentum.accelerationY = max(this->Momentum.accelerationY - this->Momentum.attenuationY, 0.0f);
		}
		else if (this->Momentum.accelerationY < 0.0f)
		{
			this->Momentum.accelerationY = min(this->Momentum.accelerationY + this->Momentum.attenuationY, 0.0f);
		}
	}
	
	/* linear relocation depending on momentary velocity */
	auto deltaX = this->Momentum.speedX;
	auto deltaY = this->Momentum.speedY;
	auto newPosition = D2D1::RectF(this->Position.left + deltaX, this->Position.top + deltaY, this->Position.right + deltaX, this->Position.bottom + deltaY);
	
	/* hit testing */
	auto additionalBoundingBoxes = this->GetBoundingBox(&myBoundingBox);
	if (deltaX < 0.0F)
	{
		/* hit something int the west? */

	}
	
	if (deltaX > 0.0F)
	{
		/* hit something in the east?
		 * we browse the environment in the order of most likely interaction,
		 * so in this case this will be next-to-right first */
		auto eastCenterTerrain = this->m_Level->GetObjectAt(this->PositionSquare.x + 1, this->PositionSquare.y, false);
		if (eastCenterTerrain != nullptr && eastCenterTerrain->m_BehaviorsWalls != ObjectBehaviors::Boring)
		{
			D2D1_RECT_F eastCenterBoundingBox;
			eastCenterTerrain->GetBoundingBox(&eastCenterBoundingBox);
			if (eastCenterBoundingBox.left < myBoundingBox.right)
			{
				auto originalWidth = newPosition.right - newPosition.left;
				auto wouldPenetrate = myBoundingBox.right - eastCenterBoundingBox.left;
				//newPosition.left = newPosition.left - wouldPenetrate;
				//newPosition.right = newPosition.left + originalWidth;

				newPosition.right = eastCenterBoundingBox.left - 1.0F;
				newPosition.left = newPosition.right - originalWidth;
			}
		}
		

	}

	if (deltaY < 0.0F)
	{
		/* hit something to the north? */

	}

	if (deltaY > 0.0F)
	{
		/* hit something in the south? */
		
	}

	this->SetPosition(newPosition);
}