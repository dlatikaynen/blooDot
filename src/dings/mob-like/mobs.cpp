#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

Player1::Player1(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(12, "Player-1", deviceResources, drawBrushes)
{
	m_Facings = Facings::Viech;
	m_Coalescing = Facings::Shy;
	m_preferredLayer = Layers::Walls;
	m_possibleLayers = Layers::Walls;
}

Platform::String^ Player1::ShouldLoadFromBitmap()
{
	return L"player-1.png";
}

Schaedel::Schaedel(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(4042, L"Sch�del", deviceResources, drawBrushes)
{
	m_Facings = Facings::Viech;
	m_Coalescing = Facings::Shy;
	m_preferredLayer = Layers::Walls;
	m_possibleLayers = Layers::Walls;
}

Platform::String^ Schaedel::ShouldLoadFromBitmap()
{
	return L"schaedel.png";
}
