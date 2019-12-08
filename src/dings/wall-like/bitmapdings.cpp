#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

HighGrass::HighGrass(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(3, "High grass", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = (Layers)(Layers::Walls | Layers::Floor);
}

Platform::String^ HighGrass::ShouldLoadFromBitmap()
{
	return L"highgrass.png";
}

Snow::Snow(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(4, "Snow area", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = (Layers)(Layers::Walls | Layers::Floor);
}

Platform::String^ Snow::ShouldLoadFromBitmap()
{
	return L"snowblock.png";
}

FloorStoneTile::FloorStoneTile(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(5, "Floor stone tiling", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = Layers::Floor;
}

Platform::String^ FloorStoneTile::ShouldLoadFromBitmap()
{
	return L"floorstonetile.png";
}

FloorRockTile::FloorRockTile(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(10, "Floor rock tiling", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = Layers::Floor;
}

Platform::String^ FloorRockTile::ShouldLoadFromBitmap()
{
	return L"floorrocktile.png";
}
