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

void Player::Update(std::shared_ptr<Level> hostLevel)
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
		/* hit something in the east? */
		// yet bullshit. must identify candidate collision victims first
		if (newPosition.right > myBoundingBox.left)
		{
			auto width = newPosition.right - newPosition.left;
			newPosition.left = newPosition.left - (newPosition.right - myBoundingBox.left);
			newPosition.right = newPosition.left + width;
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

	this->Position = newPosition;
}