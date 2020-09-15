#include "..\..\PreCompiledHeaders.h"
#include "..\mobs.h"

Dalek::Dalek(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Mob(Dings::DingIDs::Dalek, "Dalek", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Viech;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::ActiveMoving |
		ObjectBehaviors::Pullable |
		ObjectBehaviors::Pushable |
		ObjectBehaviors::Lethal;
}

Platform::String^ Dalek::ShouldLoadFromBitmap()
{
	return L"dalek.png";
}
