#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

Dalek::Dalek(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(34, "Dalek", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Viech;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
}

Platform::String^ Dalek::ShouldLoadFromBitmap()
{
	return L"dalek.png";
}
