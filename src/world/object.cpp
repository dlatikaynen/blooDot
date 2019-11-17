#include "Object.h"

Object::Object(unsigned posInLevelX, unsigned posInLevelY)
{
	this->Weed();
	this->m_positionInLevel = D2D1::Point2U(posInLevelX, posInLevelY);
}

void Object::Instantiate(Dings* templateDing)
{
	auto preferredLayer = templateDing->GetPreferredLayer();
	this->InstantiateInLayer(preferredLayer, templateDing);
}

void Object::InstantiateInLayer(Layers inLayer, Dings* templateDing)
{
	switch (inLayer)
	{
	case Layers::Floor:
		this->m_DingFloor = templateDing;
		break;

	case Layers::Walls:
		this->m_DingWalls = templateDing;
		break;

	case Layers::Rooof:
		this->m_DingRooof = templateDing;
		break;
	}

	this->m_Layers = static_cast<Layers>(this->m_Layers | inLayer);
}

void Object::Weed()
{
	this->m_DingFloor = nullptr;
	this->m_DingWalls = nullptr;
	this->m_DingRooof = nullptr;
	this->m_FacingFloor = Facings::Shy;
	this->m_FacingWalls = Facings::Shy;
	this->m_FacingRooof = Facings::Shy;
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
	switch (this->m_Layers)
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

Facings	Object::PlacementFacing()
{
	return this->m_FacingWalls;
}