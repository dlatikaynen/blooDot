#include "World.h"

#include <fstream>

Blocks::Blocks(unsigned posInLevelX, unsigned posInLevelY)
{
	this->Weed();
	this->PositionSquare = D2D1::Point2U(posInLevelX, posInLevelY);
}

Blocks::~Blocks()
{

}

void Blocks::Instantiate(std::shared_ptr<Level> placeInLevel, std::shared_ptr<Dings> templateDing, ClumsyPacking::NeighborConfiguration neighborHood)
{
	auto preferredLayer = templateDing->GetPreferredLayer();
	this->InstantiateInLayer(placeInLevel, preferredLayer, templateDing, neighborHood);
}

void Blocks::InstantiateInLayer(std::shared_ptr<Level> placeInLevel, Layers inLayer, std::shared_ptr<Dings> templateDing, ClumsyPacking::NeighborConfiguration neighborHood)
{
	auto facingVariation = templateDing->CouldCoalesce() ? ClumsyPacking::FacingFromConfiguration(neighborHood) : Facings::Shy;
	this->InstantiateInLayerFacing(placeInLevel, inLayer, templateDing, facingVariation);
}

void Blocks::InstantiateInLayerFacing(std::shared_ptr<Level> placeInLevel, Layers inLayer, std::shared_ptr<Dings> templateDing, Facings placementFacing)
{
	switch (inLayer)
	{
		case Layers::Floor:
			this->m_ObjectFloor = std::make_shared<BlockObject>(this->shared_from_this());
			this->m_ObjectFloor->InstantiateFacing(templateDing, placementFacing);
			break;

		case Layers::Walls:
			this->m_ObjectWalls = std::make_shared<BlockObject>(this->shared_from_this());
			this->m_ObjectWalls->InstantiateFacing(templateDing, placementFacing);
			break;

		case Layers::Rooof:
			this->m_ObjectRooof = std::make_shared<BlockObject>(this->shared_from_this());
			this->m_ObjectRooof->InstantiateFacing(templateDing, placementFacing);
			break;
	}
		
	this->PlaceInLevel(placeInLevel);
}

void Blocks::PlaceInLevel(std::shared_ptr<Level> hostLevel)
{
	this->m_Level = hostLevel;
	this->SetPosition(this->PositionSquare);
	if (this->m_ObjectFloor != nullptr)
	{
		this->m_ObjectFloor->PlaceInLevel();
	}

	if (this->m_ObjectWalls != nullptr)
	{
		this->m_ObjectWalls->PlaceInLevel();
	}

	if (this->m_ObjectRooof != nullptr)
	{
		this->m_ObjectRooof->PlaceInLevel();
	}
}

void Blocks::SetPosition(D2D1_POINT_2U gridPosition)
{
	this->PositionSquare.x = gridPosition.x;
	this->PositionSquare.y = gridPosition.y;
	this->Position.left = gridPosition.x * blooDot::Consts::SQUARE_WIDTH;
	this->Position.top = gridPosition.y * blooDot::Consts::SQUARE_HEIGHT;
	this->Position.right = this->Position.left + blooDot::Consts::SQUARE_WIDTH;
	this->Position.bottom = this->Position.top + blooDot::Consts::SQUARE_HEIGHT;
}

void Blocks::SetPosition(D2D1_POINT_2F pixelPosition)
{
	this->Position.left = pixelPosition.x;
	this->Position.top = pixelPosition.y;
	this->Position.right = this->Position.left + blooDot::Consts::SQUARE_WIDTH;
	this->Position.bottom = this->Position.top + blooDot::Consts::SQUARE_HEIGHT;
	this->PositionSquare.x = static_cast<int>((this->Position.left + blooDot::Consts::SQUARE_WIDTH / 2.0F) / blooDot::Consts::SQUARE_WIDTH);
	this->PositionSquare.y = static_cast<int>((this->Position.top + blooDot::Consts::SQUARE_HEIGHT / 2.0F) / blooDot::Consts::SQUARE_HEIGHT);
}

void Blocks::SetPosition(D2D1_RECT_F pixelPosition)
{
	this->Position.left = pixelPosition.left;
	this->Position.top = pixelPosition.top;
	this->Position.right = pixelPosition.right;
	this->Position.bottom = pixelPosition.bottom;
	this->PositionSquare.x = static_cast<int>((this->Position.left + blooDot::Consts::SQUARE_WIDTH / 2.0F) / blooDot::Consts::SQUARE_WIDTH);
	this->PositionSquare.y = static_cast<int>((this->Position.top + blooDot::Consts::SQUARE_HEIGHT / 2.0F) / blooDot::Consts::SQUARE_HEIGHT);
}

void Blocks::Weed()
{
	std::shared_ptr<Dings> tempDing = nullptr;
	Layers tempLayer = Layers::None;
	while (!this->WeedFromTop(&tempDing, &tempLayer));
}

// returns true, if nothing left
// Dings are placement-created, don't delete them
bool Blocks::WeedFromTop(std::shared_ptr<Dings>* dingWeeded, Layers* layerWeeded)
{
	if (this->m_ObjectRooof != nullptr)
	{
		(*dingWeeded) = this->m_ObjectRooof->GetDing();
		(*layerWeeded) = Layers::Rooof;
		this->m_ObjectRooof = nullptr;
	}
	else if (this->m_ObjectWalls != nullptr)
	{
		(*dingWeeded) = this->m_ObjectWalls->GetDing();
		(*layerWeeded) = Layers::Walls;
		this->m_ObjectWalls = nullptr;
	}
	else if (this->m_ObjectFloor != nullptr)
	{
		(*dingWeeded) = this->m_ObjectFloor->GetDing();
		(*layerWeeded) = Layers::Floor;
		this->m_ObjectFloor = nullptr;
	}
	else
	{
		(*dingWeeded) = nullptr;
		(*layerWeeded) = Layers::None;
	}

	return this->m_ObjectRooof == nullptr && this->m_ObjectWalls == nullptr && this->m_ObjectFloor == nullptr;
}

Layers Blocks::GetTopmostPopulatedLayer()
{
	if (this->m_ObjectRooof != nullptr)
	{
		return Layers::Rooof;
	}

	if (this->m_ObjectWalls != nullptr)
	{
		return Layers::Walls;
	}

	if (this->m_ObjectFloor != nullptr)
	{
		return Layers::Floor;
	}

	return Layers::None;
}

Platform::String^ Blocks::GetName()
{
	std::wstring names;
	if (this->m_ObjectFloor != nullptr)
	{
		if (names.length() > 0)
		{
			names.append(L"\r\n");
		}

		names.append(this->m_ObjectFloor->GetDing()->Name()->Data());
	}

	if (this->m_ObjectWalls != nullptr)
	{
		if (names.length() > 0)
		{
			names.append(L"\r\n");
		}

		names.append(this->m_ObjectWalls->GetDing()->Name()->Data());
	}

	if (this->m_ObjectRooof != nullptr)
	{
		if (names.length() > 0)
		{
			names.append(L"\r\n");
		}

		names.append(this->m_ObjectRooof->GetDing()->Name()->Data());
	}

	return ref new Platform::String(names.c_str());
}

Layers Blocks::GetLayers()
{
	auto layersFound = Layers::None;
	if (this->m_ObjectFloor != nullptr)
	{
		layersFound |= Layers::Floor;
	}

	if (this->m_ObjectWalls != nullptr)
	{
		layersFound |= Layers::Walls;
	}

	if (this->m_ObjectRooof != nullptr)
	{
		layersFound |= Layers::Rooof;
	}

	return layersFound;
}

std::shared_ptr<BlockObject> Blocks::GetObject(Layers ofLayer)
{
	switch (ofLayer)
	{
		case Layers::Floor:
			return this->m_ObjectFloor;

		case Layers::Walls:
			return this->m_ObjectWalls;

		case Layers::Rooof:
			return this->m_ObjectRooof;
	}

	return nullptr;
}

void Blocks::DesignSaveToFile(std::ofstream* toFile, Layers ofLayer)
{
	auto thisObject = this->GetObject(ofLayer);
	auto thisDing = thisObject->GetDing();
	if (thisDing == nullptr)
	{
		toFile->write((char*)0, sizeof(byte));
	}
	else 
	{
		auto dingID = thisDing->ID();
		auto dingFacing = thisObject->PlacementFacing();
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
