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
	this->m_Facings = Facings::Viech;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
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
	this->m_Facings = Facings::Viech;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
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

FlameGhost::FlameGhost(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Mob(Dings::DingIDs::FlameGhost, L"Flammengeist", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Viech;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
	this->m_extentOnSheet = D2D1::SizeU(3, 3);
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::Shootable |
		ObjectBehaviors::ActiveMoving |
		ObjectBehaviors::Lethal;
}

void FlameGhost::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, const D2D1_RECT_F *rect)
{
	MFARGB colrect{ 11, 137, 221, 255 };
	MFARGB colring{ 12, 12, 12, 255 };
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl;
	auto rimRadius = blooDot::Consts::GOLDEN_RATIO * (this->m_extentOnSheet.width * blooDot::Consts::SQUARE_HEIGHT / 2.f);
	D2D1_ELLIPSE elli = D2D1::Ellipse(D2D1::Point2F(rect->left + (rect->right - rect->left) / 2.0f, rect->top + (rect->bottom - rect->top) / 2.0f), rimRadius, rimRadius);
	brusherl = m_Brushes->WannaHave(drawTo, colrect);
	auto rimBrush = brusherl.Get();
	auto rendEr = drawTo.Get();
	rendEr->FillEllipse(elli, rimBrush);
	
	/* flame spikes */
	auto innerRadius = rimRadius;
	auto outerRadius = rimRadius + 15.f;
	auto twoPi = static_cast<float>(M_PI) * 2.f;
	auto piStep = twoPi / 11.f;
	auto endTreshold = twoPi - (piStep / 2.f);
	for (float roundRobin = 0.f; roundRobin < endTreshold; roundRobin += piStep)
	{
		auto midpointAngle = roundRobin + (piStep / 2.f);
		auto endpointAngle = roundRobin + piStep;
		rendEr->DrawLine(
			D2D1::Point2F(elli.point.x + innerRadius * cosf(roundRobin), elli.point.y + innerRadius * sinf(roundRobin)),
			D2D1::Point2F(elli.point.x + outerRadius * cosf(midpointAngle), elli.point.y + outerRadius * sinf(midpointAngle)),
			rimBrush, 
			1.5f
		);

		rendEr->DrawLine(
			D2D1::Point2F(elli.point.x + outerRadius * cosf(midpointAngle), elli.point.y + outerRadius * sinf(midpointAngle)),
			D2D1::Point2F(elli.point.x + innerRadius * cosf(endpointAngle), elli.point.y + innerRadius * sinf(endpointAngle)),
			rimBrush,
			1.5f
		);
	}

	/* interior last */
	brusherl = m_Brushes->WannaHave(drawTo, colring);
	auto ringBrush = brusherl.Get();
	elli.radiusX = elli.radiusY = (elli.radiusX - 2.5f);
	rendEr->DrawEllipse(elli, ringBrush, 0.9f);
	rendEr->DrawLine(D2D1::Point2F(elli.point.x + 0.5f, elli.point.y - 5.0f), D2D1::Point2F(elli.point.x + 0.5f, elli.point.y + 5.0f), ringBrush, 1.5f);
	rendEr->DrawLine(D2D1::Point2F(elli.point.x - 2.5f, elli.point.y - 3.0f), D2D1::Point2F(elli.point.x + 0.5f, elli.point.y - 5.0f), ringBrush, 1.5f);
}
