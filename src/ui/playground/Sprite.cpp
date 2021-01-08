#include <Windows.h>
#include <iostream>
#include <sstream>

#include "src\world\World.h"
#include "src\world\Level.h"
#include "Sprite.h"

Sprite::Sprite()
{
	BlockObject::BlockObject((std::shared_ptr<Blocks>)nullptr);
	this->Momentum.speedX = 0.f;
	this->Momentum.speedY = 0.f;
	this->Momentum.accelerationX = 0.f;
	this->Momentum.accelerationY = 0.f;
	this->Momentum.attenuationX = 0.f;
	this->Momentum.attenuationY = 0.f;
	this->m_timeAccrued = 0.f;
	this->m_timeForRotation = false;
}

Sprite::~Sprite()
{
}

void Sprite::InitializeIn(Dings::DingIDs dingID, Platform::String^ instanceName, std::shared_ptr<Level> inLevel, unsigned positionInLevelX, unsigned positionInLevelY, Facings mobFacing)
{
	this->Name = instanceName;
	this->SetPosition(D2D1::Point2U(positionInLevelX, positionInLevelY));
	auto playerTemplate = inLevel->GetDing(dingID);
	this->InstantiateFacing(playerTemplate, mobFacing);
	this->m_orientationCurrent = Dings::HeadingFromFacing(mobFacing);
	this->m_Level = inLevel;
	this->PlaceInLevel();
}

void Sprite::PushX(float accelerationRate, float attenuationRate, float gripFactor, float mediumViscosity)
{
	this->Momentum.accelerationX = accelerationRate;
	this->Momentum.attenuationX = attenuationRate;
	this->Momentum.gripFactor = gripFactor;
	this->Momentum.mediumViscosity = mediumViscosity;
}

void Sprite::PushY(float accelerationRate, float attenuationRate, float gripFactor, float mediumViscosity)
{
	this->Momentum.accelerationY = accelerationRate;
	this->Momentum.attenuationY = attenuationRate;
	this->Momentum.gripFactor = gripFactor;
	this->Momentum.mediumViscosity = mediumViscosity;
}

std::shared_ptr<BlockObject> Sprite::Update(float timeTotal, float timeDelta)
{
	D2D1_RECT_F myBoundingBox;
	std::shared_ptr<BlockObject> collidedWith = nullptr;
	this->m_timeAccrued += timeDelta;
	if (!this->m_timeForRotation && this->m_timeAccrued > .1f)
	{
		this->m_timeForRotation = true;
	}

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
	auto deltaX = abs(this->Momentum.speedX) > MobMomentum::speedCapX ? std::copysignf(MobMomentum::speedCapX, this->Momentum.speedX) : this->Momentum.speedX;
	auto deltaY = abs(this->Momentum.speedY) > MobMomentum::speedCapY ? std::copysignf(MobMomentum::speedCapY, this->Momentum.speedY) : this->Momentum.speedY;	
	auto newPosition = D2D1::RectF(
		this->Position.left + deltaX,
		this->Position.top + deltaY,
		this->Position.right + deltaX,
		this->Position.bottom + deltaY
	);
	
	/* orient the sprite based on the intented vector */
	if (abs(deltaX) > 0.1F || abs(deltaY) > 0.1F)
	{		
		this->m_orientationCurrent = Dings::HeadingFromVector(deltaX, deltaY);

#if _DEBUG
		std::wostringstream os_;
		os_ << this->m_orientationCurrent << " (" << deltaX << ", " << deltaY << ")" << std::endl ;
		OutputDebugStringW(os_.str().c_str());
#endif

		this->SetMobRotation(this->m_orientationCurrent);
	}

	/* hit testing */
	auto additionalBoundingBoxes = this->GetBoundingBox(&myBoundingBox);
	if (deltaX < 0.0F)
	{
		/* hit something int the west? */
		auto westCenterBlocks = this->m_Level->GetBlocksAt(this->PositionSquare.x - 1, this->PositionSquare.y, false);
		auto westCenterTerrain = westCenterBlocks == nullptr ? nullptr : westCenterBlocks->GetObject(Layers::Walls);
		if (westCenterTerrain != nullptr && westCenterTerrain->m_Behaviors != ObjectBehaviors::Boring)
		{
			D2D1_RECT_F westCenterBoundingBox;
			westCenterTerrain->GetBoundingBox(&westCenterBoundingBox);
			if (westCenterBoundingBox.right > myBoundingBox.left)
			{
				auto originalWidth = newPosition.right - newPosition.left;
				auto wouldPenetrate = westCenterBoundingBox.right - myBoundingBox.left;
				newPosition.left = westCenterBoundingBox.right + 1.0F;
				newPosition.right = newPosition.left + originalWidth;
				if (westCenterTerrain->m_Behaviors & ObjectBehaviors::Pushable)
				{
					this->PushTheWall(westCenterTerrain, Facings::West);
				}
				else if (westCenterTerrain->m_Behaviors & ObjectBehaviors::Solid)
				{
					this->Momentum.HitTheWall(Facings::West);
				}
				else
				{
					collidedWith = westCenterTerrain;
				}
			}
		}
	}
	
	if (deltaX > 0.0F)
	{
		/* hit something in the east?
		 * we browse the environment in the order of most likely interaction,
		 * so in this case this will be next-to-right first */
		auto eastCenterBlocks = this->m_Level->GetBlocksAt(this->PositionSquare.x + 1, this->PositionSquare.y, false);
		auto eastCenterTerrain = eastCenterBlocks==nullptr ? nullptr : eastCenterBlocks->GetObject(Layers::Walls);
		if (eastCenterTerrain != nullptr && eastCenterTerrain->m_Behaviors != ObjectBehaviors::Boring)
		{
			D2D1_RECT_F eastCenterBoundingBox;
			eastCenterTerrain->GetBoundingBox(&eastCenterBoundingBox);
			if (eastCenterBoundingBox.left < myBoundingBox.right)
			{
				auto originalWidth = newPosition.right - newPosition.left;
				auto wouldPenetrate = myBoundingBox.right - eastCenterBoundingBox.left;
				newPosition.right = eastCenterBoundingBox.left - 1.0F;
				newPosition.left = newPosition.right - originalWidth;
				if (eastCenterTerrain->m_Behaviors & ObjectBehaviors::Pushable)
				{
					this->PushTheWall(eastCenterTerrain, Facings::East);
				}
				else if (eastCenterTerrain->m_Behaviors & ObjectBehaviors::Solid)
				{
					this->Momentum.HitTheWall(Facings::East);
				}
				else
				{
					collidedWith = eastCenterTerrain;
				}
			}
		}		
	}

	if (deltaY < 0.0F)
	{
		/* hit something to the north? */
		auto northCenterBlocks = this->m_Level->GetBlocksAt(this->PositionSquare.x, this->PositionSquare.y - 1, false);
		auto northCenterTerrain = northCenterBlocks == nullptr ? nullptr : northCenterBlocks->GetObject(Layers::Walls);
		if (northCenterTerrain != nullptr && northCenterTerrain->m_Behaviors != ObjectBehaviors::Boring)
		{
			D2D1_RECT_F northCenterBoundingBox;
			northCenterTerrain->GetBoundingBox(&northCenterBoundingBox);
			if (northCenterBoundingBox.bottom > myBoundingBox.top)
			{
				auto originalHeight = newPosition.bottom - newPosition.top;
				auto wouldPenetrate = northCenterBoundingBox.bottom - myBoundingBox.top;
				newPosition.top = northCenterBoundingBox.bottom + 1.0F;
				newPosition.bottom = newPosition.top + originalHeight;
				if (northCenterTerrain->m_Behaviors & ObjectBehaviors::Pushable)
				{
					this->PushTheWall(northCenterTerrain, Facings::North);
				}
				else if (northCenterTerrain->m_Behaviors & ObjectBehaviors::Solid)
				{
					this->Momentum.HitTheWall(Facings::North);
				}
				else
				{
					collidedWith = northCenterTerrain;
				}
			}
		}
	}

	if (deltaY > 0.0F)
	{
		/* hit something in the south? */
		auto southCenterBlocks = this->m_Level->GetBlocksAt(this->PositionSquare.x, this->PositionSquare.y + 1, false);
		auto southCenterTerrain = southCenterBlocks == nullptr ? nullptr : southCenterBlocks->GetObject(Layers::Walls);
		if (southCenterTerrain != nullptr && southCenterTerrain->m_Behaviors != ObjectBehaviors::Boring)
		{
			D2D1_RECT_F southCenterBoundingBox;
			southCenterTerrain->GetBoundingBox(&southCenterBoundingBox);
			if (southCenterBoundingBox.top < myBoundingBox.bottom)
			{
				auto originalHeight = newPosition.bottom - newPosition.top;
				auto wouldPenetrate = myBoundingBox.bottom - southCenterBoundingBox.top;
				newPosition.bottom = southCenterBoundingBox.top - 1.0F;
				newPosition.top = newPosition.bottom - originalHeight;
				if (southCenterTerrain->m_Behaviors & ObjectBehaviors::Pushable)
				{
					this->PushTheWall(southCenterTerrain, Facings::South);
				}
				else if (southCenterTerrain->m_Behaviors & ObjectBehaviors::Solid)
				{
					this->Momentum.HitTheWall(Facings::South);
				}
				else
				{
					collidedWith = southCenterTerrain;
				}
			}
		}
	}

	this->SetPosition(newPosition);
	return collidedWith;
}

bool Sprite::TimingRotationPending()
{
	return this->m_timeForRotation;
}

bool Sprite::TimingRotationClear()
{
	if (this->m_timeForRotation)
	{
		this->m_timeForRotation = false;
		this->m_timeAccrued = 0.f;
		return true;
	}

	return false;
}

void Sprite::PushTheWall(std::shared_ptr<BlockObject> pushableObject, Facings towardsDirection)
{
	/* 1. we are an actively moving mob.
	 * can we push that, given our own momentum? */

	/* if we can, it attenuates our momentum */

	// F = m * a


	/* if we cannot, it acts just like any other solid */
	this->Momentum.HitTheWall(towardsDirection);

}
