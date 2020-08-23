#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

FloorStoneTile::FloorStoneTile(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(Dings::DingIDs::FloorTilingStone, "Floor stone tiling", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = Layers::Floor;
	this->m_Behaviors = ObjectBehaviors::Solid;
}

Platform::String^ FloorStoneTile::ShouldLoadFromBitmap()
{
	return L"floorstonetile.png";
}

FloorRockTile::FloorRockTile(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(Dings::DingIDs::FloorTilingRock, "Floor rock tiling", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = Layers::Floor;
	this->m_Behaviors = ObjectBehaviors::Solid;
}

Platform::String^ FloorRockTile::ShouldLoadFromBitmap()
{
	return L"floorrocktile.png";
}