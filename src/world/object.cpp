#include "Object.h"
#include <fstream>

Object::Object(unsigned posInLevelX, unsigned posInLevelY)
{
	this->Weed();
	this->m_positionInLevel = D2D1::Point2U(posInLevelX, posInLevelY);
}

void Object::Instantiate(Dings* templateDing, ClumsyPacking::NeighborConfiguration neighborHood)
{
	auto preferredLayer = templateDing->GetPreferredLayer();
	this->InstantiateInLayer(preferredLayer, templateDing, neighborHood);
}

void Object::InstantiateInLayer(Layers inLayer, Dings* templateDing, ClumsyPacking::NeighborConfiguration neighborHood)
{
	auto facingVariation = templateDing->CouldCoalesce() ? ClumsyPacking::FacingFromConfiguration(neighborHood) : Facings::Shy;
	this->InstantiateInLayer(inLayer, templateDing, facingVariation);
}

void Object::InstantiateInLayer(Layers inLayer, Dings* templateDing, Facings placementFacing)
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

void Object::Weed()
{
	Dings* tempDing = nullptr;
	Layers tempLayer = Layers::None;
	while (!this->WeedFromTop(&tempDing, &tempLayer));
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
		if ((dingID & 0x80) || dingFacing != Facings::Shy)
		{
			byte msb = 0x80 | (dingID & 0x7f);
			toFile->write((char*)&msb, sizeof(byte));
			byte lsb = static_cast<byte>(dingFacing);
			toFile->write((char*)&lsb, sizeof(byte));
		}
		else
		{
			dingID = dingID & 0x7f;
			toFile->write((char*)&dingID, sizeof(uint32));
		}
	}
}
