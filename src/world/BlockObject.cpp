#include "World.h"
#include "..\dings\ObjectProperties.h"

#include <fstream>

BlockObject::BlockObject(std::shared_ptr<Blocks> container)
{
	this->m_objectProperties = nullptr;
	this->m_blocksContainer = container;
	this->m_boundingBoxes = nullptr;
	this->m_Behaviors = ObjectBehaviors::Boring;
}

BlockObject::~BlockObject()
{
	if (this->m_boundingBoxes != nullptr)
	{
		delete this->m_boundingBoxes;
	}
}

void BlockObject::Instantiate(std::shared_ptr<Dings> templateDing, ClumsyPacking::NeighborConfiguration neighborHood)
{
	auto facingVariation = templateDing->CouldCoalesce() ? ClumsyPacking::FacingFromConfiguration(neighborHood) : Facings::Shy;
	this->InstantiateFacing(templateDing, facingVariation);
}

void BlockObject::InstantiateFacing(std::shared_ptr<Dings> templateDing, Facings placementFacing)
{
	this->m_Ding = templateDing;
	this->m_Facing = placementFacing;
}

void BlockObject::SetAnchor(std::shared_ptr<BlockObject> fromOrigin)
{
	if (this->m_anchorBlock == nullptr)
	{
		this->m_anchorBlock = fromOrigin;
	}
}

void BlockObject::ClearAnchor()
{
	this->m_anchorBlock = nullptr;
}

void BlockObject::PlaceInLevel()
{		
	if (this->m_Ding != nullptr)
	{
		this->m_boundingBox = this->m_Ding->GetBoundingOuterRim();
		if (this->m_Ding->AvailableFacings() == Facings::Viech && this->m_Facing != Facings::Shy)
		{
			this->SetMobRotation(Dings::HeadingFromFacing(this->m_Facing));
		}
	}

	if (this->m_blocksContainer != nullptr)
	{
		this->SetPosition(this->m_blocksContainer->Position);
	}
}

void BlockObject::SetPosition(D2D1_POINT_2U gridPosition)
{
	this->PositionSquare.x = gridPosition.x;
	this->PositionSquare.y = gridPosition.y;
	this->Position.left = gridPosition.x * blooDot::Consts::SQUARE_WIDTH;
	this->Position.top = gridPosition.y * blooDot::Consts::SQUARE_HEIGHT;
	this->Position.right = this->Position.left + blooDot::Consts::SQUARE_WIDTH;
	this->Position.bottom = this->Position.top + blooDot::Consts::SQUARE_HEIGHT;
}

void BlockObject::SetPosition(D2D1_POINT_2F pixelPosition)
{
	this->Position.left = pixelPosition.x;
	this->Position.top = pixelPosition.y;
	this->Position.right = this->Position.left + blooDot::Consts::SQUARE_WIDTH;
	this->Position.bottom = this->Position.top + blooDot::Consts::SQUARE_HEIGHT;
	this->PositionSquare.x = static_cast<int>((this->Position.left + blooDot::Consts::SQUARE_WIDTH / 2.0F) / blooDot::Consts::SQUARE_WIDTH);
	this->PositionSquare.y = static_cast<int>((this->Position.top + blooDot::Consts::SQUARE_HEIGHT / 2.0F) / blooDot::Consts::SQUARE_HEIGHT);
}

void BlockObject::SetPosition(D2D1_RECT_F pixelPosition)
{
	this->Position.left = pixelPosition.left;
	this->Position.top = pixelPosition.top;
	this->Position.right = pixelPosition.right;
	this->Position.bottom = pixelPosition.bottom;
	this->PositionSquare.x = static_cast<int>((this->Position.left + blooDot::Consts::SQUARE_WIDTH / 2.0F) / blooDot::Consts::SQUARE_WIDTH);
	this->PositionSquare.y = static_cast<int>((this->Position.top + blooDot::Consts::SQUARE_HEIGHT / 2.0F) / blooDot::Consts::SQUARE_HEIGHT);
}

OrientabilityIndexRotatory BlockObject::GetMobRotation()
{
	return this->m_orientationCurrent;
}

void BlockObject::SetMobRotation(OrientabilityIndexRotatory rotationDent)
{
	this->m_orientationCurrent = rotationDent;
	auto spriteOnSheet = this->m_Ding->GetSheetPlacement(rotationDent);
	auto sizeOnSheet = this->m_Ding->GetExtentOnSheet();
	this->m_spriteSourceRect.left = spriteOnSheet.x * blooDot::Consts::SQUARE_WIDTH;
	this->m_spriteSourceRect.top = spriteOnSheet.y * blooDot::Consts::SQUARE_HEIGHT;
	this->m_spriteSourceRect.right = this->m_spriteSourceRect.left + blooDot::Consts::SQUARE_WIDTH * sizeOnSheet.width;
	this->m_spriteSourceRect.bottom = this->m_spriteSourceRect.top + blooDot::Consts::SQUARE_HEIGHT * sizeOnSheet.height;
}

void BlockObject::SetupRuntimeState()
{
	auto baseDing = this->GetDing();
	if (baseDing != nullptr)
	{
		this->m_Behaviors = baseDing->GetInherentBehaviors();
	}
}

std::shared_ptr<Dings> BlockObject::GetDing()
{
	return this->m_Ding;
}

Facings	BlockObject::PlacementFacing()
{
	return this->m_Facing;
}

void BlockObject::RotateInPlace(bool inverseDirection)
{
	this->AdjustFacing(Dings::RotateFromFacing(this->m_Facing, inverseDirection));
}

void BlockObject::AdjustFacing(Facings shouldBeFacing)
{
	this->m_Facing = shouldBeFacing;
}

bool BlockObject::GetBoundingBox(_Out_ D2D1_RECT_F* boundingBox)
{	
	*boundingBox = D2D1::RectF(
		this->m_boundingBox.left + this->Position.left,
		this->m_boundingBox.top + this->Position.top,
		this->m_boundingBox.right + this->Position.left,
		this->m_boundingBox.bottom + this->Position.top
	);

	if (this->m_boundingBoxes == nullptr)
	{
		return false;
	}
	else
	{
		this->m_boundingBoxIter = this->m_boundingBoxes->begin();
		return true;
	}
}

bool BlockObject::GetBoundingBoxNext(_Out_ D2D1_RECT_F* boundingBox)
{
	auto relativeBox =  *(this->m_boundingBoxIter);
	*boundingBox = D2D1::RectF(
		relativeBox.left + this->Position.left,
		relativeBox.top + this->Position.top,
		relativeBox.right + this->Position.left,
		relativeBox.bottom + this->Position.top
	);

	++this->m_boundingBoxIter;
	return this->m_boundingBoxIter != this->m_boundingBoxes->end();
}
