#include "..\..\PreCompiledHeaders.h"
#include "..\mobs.h"

Mob::Mob(Dings::DingIDs dingID, Platform::String^ dingName, std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(dingID, dingName, deviceResources, drawBrushes)
{
}

bool Mob::IsMob()
{
	return true;
}

Player1::Player1(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Mob(Dings::DingIDs::Player, "Player-1", deviceResources, drawBrushes)
{
	m_Facings = Facings::Viech;
	m_Coalescing = Facings::Shy;
	m_preferredLayer = Layers::Walls;
	m_possibleLayers = Layers::Walls;
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::Pushable |
		ObjectBehaviors::Pullable |
		ObjectBehaviors::ActiveMoving |
		ObjectBehaviors::CanPutStuffIn;

	/* semi-complex bounding here */
	this->m_Bounding = std::make_shared<Bounding>();
	this->m_Bounding->OuterRim = D2D1::RectF(3, 9, 47, 40);

	BoundingGeometry headCircle;
	headCircle.Shape = BoundingGeometry::Primitive::Circle;
	headCircle.Bounds = D2D1::RectF(10, 9, 40, 40);
	this->m_Bounding->Geometries.push_back(headCircle);

	BoundingGeometry innerRect;
	innerRect.Shape = BoundingGeometry::Primitive::Rectangle;
	innerRect.Bounds = D2D1::RectF(3, 16, 47, 28);
	this->m_Bounding->Geometries.push_back(innerRect);
}

Platform::String^ Player1::ShouldLoadFromBitmap()
{
	return L"player-1.png";
}

Platform::String^ Player2::ShouldLoadFromBitmap()
{
	return L"player-2.png";
}

Platform::String^ Player3::ShouldLoadFromBitmap()
{
	return L"player-3.png";
}

Platform::String^ Player4::ShouldLoadFromBitmap()
{
	return L"player-4.png";
}

Schaedel::Schaedel(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Mob(Dings::DingIDs::Schaedel, L"Schädel", deviceResources, drawBrushes)
{
	m_Facings = Facings::Viech;
	m_Coalescing = Facings::Shy;
	m_preferredLayer = Layers::Walls;
	m_possibleLayers = Layers::Walls;
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::Shootable |
		ObjectBehaviors::ActiveMoving |
		ObjectBehaviors::Lethal;
}

Platform::String^ Schaedel::ShouldLoadFromBitmap()
{
	return L"schaedel.png";
}
