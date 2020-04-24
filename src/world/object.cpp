#include "Object.h"
#include "..\dings\ObjectProperties.h"

#include <fstream>

Object::Object(unsigned posInLevelX, unsigned posInLevelY)
{
	this->m_boundingBoxes = nullptr;
	this->Weed();
	this->m_positionInLevel = D2D1::Point2U(posInLevelX, posInLevelY);
	this->m_BehaviorsFloor = ObjectBehaviors::Boring;
	this->m_BehaviorsWalls = ObjectBehaviors::Boring;
	this->m_BehaviorsRooof = ObjectBehaviors::Boring;
}

Object::~Object()
{
	if (this->m_boundingBoxes != nullptr)
	{
		delete this->m_boundingBoxes;
	}
}

void Object::Instantiate(Dings* templateDing, ClumsyPacking::NeighborConfiguration neighborHood)
{
	auto preferredLayer = templateDing->GetPreferredLayer();
	this->InstantiateInLayer(preferredLayer, templateDing, neighborHood);
}

void Object::InstantiateInLayer(Layers inLayer, Dings* templateDing, ClumsyPacking::NeighborConfiguration neighborHood)
{
	auto facingVariation = templateDing->CouldCoalesce() ? ClumsyPacking::FacingFromConfiguration(neighborHood) : Facings::Shy;
	this->InstantiateInLayerFacing(inLayer, templateDing, facingVariation);
}

void Object::InstantiateInLayerFacing(Layers inLayer, Dings* templateDing, Facings placementFacing)
{
	switch (inLayer)
	{
		case Layers::Floor:
			this->m_DingFloor = templateDing;
			this->m_FacingFloor = placementFacing;
			break;

		case Layers::Walls:
			this->m_DingWalls = templateDing;
			this->m_FacingWalls = placementFacing;
			break;

		case Layers::Rooof:
			this->m_DingRooof = templateDing;
			this->m_FacingRooof = placementFacing;
			break;
	}
	
	this->m_Layers = static_cast<Layers>(this->m_Layers | inLayer);
}

void Object::PlaceInLevel(std::shared_ptr<Level> hostLevel)
{
	this->m_Level = hostLevel;
}

void Object::Weed()
{
	Dings* tempDing = nullptr;
	Layers tempLayer = Layers::None;
	while (!this->WeedFromTop(&tempDing, &tempLayer));
	if (this->m_boundingBoxes != nullptr)
	{
		delete this->m_boundingBoxes;
		this->m_boundingBoxes = nullptr;
	}
}

// returns true, if nothing left
// Dings are placement-created, don't delete them
bool Object::WeedFromTop(Dings** dingWeeded, Layers* layerWeeded)
{
	if (this->m_DingRooof != nullptr)
	{
		(*dingWeeded) = this->m_DingRooof;
		(*layerWeeded) = Layers::Rooof;
		this->m_DingRooof = nullptr;
		this->m_FacingRooof = Facings::Shy;
		this->m_Layers = static_cast<Layers>(this->m_Layers & ~Layers::Rooof);
	}
	else if (this->m_DingWalls != nullptr)
	{
		(*dingWeeded) = this->m_DingWalls;
		(*layerWeeded) = Layers::Walls;
		this->m_DingWalls = nullptr;
		this->m_FacingWalls = Facings::Shy;
		this->m_Layers = static_cast<Layers>(this->m_Layers & ~Layers::Walls);
	}
	else if (this->m_DingFloor != nullptr)
	{
		(*dingWeeded) = this->m_DingFloor;
		(*layerWeeded) = Layers::Floor;
		this->m_DingFloor = nullptr;
		this->m_FacingFloor = Facings::Shy;
		this->m_Layers = static_cast<Layers>(this->m_Layers & ~Layers::Floor);
	}
	else
	{
		(*dingWeeded) = nullptr;
		(*layerWeeded) = Layers::None;
	}

	return this->m_DingRooof == nullptr && this->m_DingWalls == nullptr && this->m_DingFloor == nullptr;
}

void Object::SetupRuntimeState(Dings* floorDing, Dings* wallsDing, Dings* rooofDing)
{
	if (floorDing != nullptr)
	{
		this->m_BehaviorsFloor = floorDing->GetInherentBehaviors();
	}

	if (wallsDing != nullptr)
	{
		this->m_BehaviorsWalls = wallsDing->GetInherentBehaviors();
	}

	if (rooofDing != nullptr)
	{
		this->m_BehaviorsRooof = rooofDing->GetInherentBehaviors();
	}
}

Layers Object::GetTopmostPopulatedLayer()
{
	if (this->m_DingRooof != nullptr)
	{
		return Layers::Rooof;
	}

	if (this->m_DingWalls != nullptr)
	{
		return Layers::Walls;
	}

	if (this->m_DingFloor != nullptr)
	{
		return Layers::Floor;
	}

	return Layers::None;
}

Platform::String^ Object::GetName()
{
	std::wstring names;
	if (this->m_DingFloor != nullptr)
	{
		if (names.length() > 0)
		{
			names.append(L"\r\n");
		}

		names.append(this->m_DingFloor->Name()->Data());
	}

	if (this->m_DingWalls != nullptr)
	{
		if (names.length() > 0)
		{
			names.append(L"\r\n");
		}

		names.append(this->m_DingWalls->Name()->Data());
	}

	if (this->m_DingRooof != nullptr)
	{
		if (names.length() > 0)
		{
			names.append(L"\r\n");
		}

		names.append(this->m_DingRooof->Name()->Data());
	}

	return ref new Platform::String(names.c_str());
}

Layers Object::GetLayers()
{
	return m_Layers;
}

Dings* Object::GetDing(Layers ofLayer)
{
	switch (ofLayer)
	{
	case Layers::Floor:
		return this->m_DingFloor;

	case Layers::Walls:
		return this->m_DingWalls;

	case Layers::Rooof:
		return this->m_DingRooof;
	}

	return nullptr;
}

Facings	Object::PlacementFacing(Layers ofLayer)
{
	switch (ofLayer)
	{
	case Layers::Floor:
		return this->m_FacingFloor;

	case Layers::Walls:
		return this->m_FacingWalls;

	case Layers::Rooof:
		return this->m_FacingRooof;
	}

	return Facings::Shy;
}

void Object::RotateInPlace(Layers inLayer, bool inverseDirection)
{
	switch (inLayer)
	{
	case Layers::Floor:
		this->AdjustFacing(inLayer, Dings::RotateFromFacing(this->m_FacingFloor, inverseDirection));
		break;

	case Layers::Walls:
		this->AdjustFacing(inLayer, Dings::RotateFromFacing(this->m_FacingWalls, inverseDirection));
		break;

	case Layers::Rooof:
		this->AdjustFacing(inLayer, Dings::RotateFromFacing(this->m_FacingRooof, inverseDirection));
		break;
	}
}

void Object::AdjustFacing(Layers inLayer, Facings shouldBeFacing)
{
	switch (inLayer)
	{
	case Layers::Floor:
		this->m_FacingFloor = shouldBeFacing;
		break;
		
	case Layers::Walls:
		this->m_FacingWalls = shouldBeFacing;
		break;

	case Layers::Rooof:
		this->m_FacingRooof = shouldBeFacing;
		break;
	}
}

void Object::DesignSaveToFile(std::ofstream* toFile, Layers ofLayer)
{
	auto thisDing = this->GetDing(ofLayer);
	if (thisDing == nullptr)
	{
		toFile->write((char*)0, sizeof(byte));
	}
	else 
	{
		auto dingID = thisDing->ID();
		auto dingFacing = this->PlacementFacing(ofLayer);
		bool isExtendedDing = static_cast<unsigned>(dingID) > 0x3f;
		bool hasPlacementFacing = dingFacing != Facings::Shy;
		byte prefix = (hasPlacementFacing ? 1 : 0) << 7 | (isExtendedDing ? 1 : 0) << 6 | (static_cast<unsigned>(dingID) & 0x3f);
		toFile->write((char*)&prefix, sizeof(byte));
		if (isExtendedDing)
		{
			byte dingMSB = (static_cast<unsigned>(dingID) & 0xffc0) >> 6;
			toFile->write((char*)&dingMSB, sizeof(byte));
		}
		
		if(hasPlacementFacing)
		{		
			unsigned int facingVal = static_cast<unsigned int>(dingFacing);
			toFile->write((char*)&facingVal, sizeof(unsigned int));
		}
	}
}

bool Object::GetBoundingBox(_Out_ D2D1_RECT_F* boundingBox)
{
	*boundingBox = this->m_boundingBox;
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

bool Object::GetBoundingBoxNext(_Out_ D2D1_RECT_F* boundingBox)
{
	*boundingBox = *(this->m_boundingBoxIter);
	++this->m_boundingBoxIter;
	return this->m_boundingBoxIter != this->m_boundingBoxes->end();
}
