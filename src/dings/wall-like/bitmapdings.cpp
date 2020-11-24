#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

HighGrass::HighGrass(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Dings(Dings::DingIDs::GrassHigh, "High grass", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = (Layers)(Layers::Walls | Layers::Floor);
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::CreatesDrag |
		ObjectBehaviors::ImpairsVisibility;
}

Platform::String^ HighGrass::ShouldLoadFromBitmap()
{
	return L"highgrass.png";
}

Snow::Snow(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Dings(Dings::DingIDs::SnowTile, "Snow area", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = (Layers)(Layers::Walls | Layers::Floor);
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::CreatesDrag;
}

Platform::String^ Snow::ShouldLoadFromBitmap()
{
	return L"snowblock.png";
}

BarrelIndigo::BarrelIndigo(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Dings(Dings::DingIDs::BarrelIndigo, "Indigo barrel", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::Takeable;
}

Platform::String^ BarrelIndigo::ShouldLoadFromBitmap()
{
	return L"barrel-indigo.png";
}

BarrelWooden::BarrelWooden(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Dings(Dings::DingIDs::BarrelWood, "Wooden barrel", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::Takeable;
}

Platform::String^ BarrelWooden::ShouldLoadFromBitmap()
{
	return L"barrel-wooden.png";
}
