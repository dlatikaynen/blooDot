#include "..\..\PreCompiledHeaders.h"
#include "..\mobs.h"

constexpr float Dings::WEDGE_STROKE = 1.5f;
constexpr float Dings::HAIRLINE = 0.72f;

Mob::Mob(Dings::DingIDs dingID, Platform::String^ dingName, std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Dings(dingID, dingName, deviceResources, drawBrushes)
{
}

bool Mob::IsMob()
{
	return true;
}

Player1::Player1(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Mob(Dings::DingIDs::Player, "Player-1", deviceResources, drawBrushes)
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

Schaedel::Schaedel(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Mob(Dings::DingIDs::Schaedel, L"Schädel", deviceResources, drawBrushes)
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

FlameGhost::FlameGhost(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Mob(Dings::DingIDs::FlameGhost, L"Flammengeist", deviceResources, drawBrushes)
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

	this->rimRadius = blooDot::Consts::GOLDEN_RATIO * (this->m_extentOnSheet.width * blooDot::Consts::SQUARE_HEIGHT / 2.f) - 1.8f;
	this->innerRadius = this->rimRadius + 1.8f;
	this->outerRadius = this->rimRadius + 15.f;
	this->m_Bounding = std::make_shared<Bounding>();
	auto midPointX = (this->m_extentOnSheet.width * blooDot::Consts::SQUARE_WIDTH) / 2.f;
	auto midPointY = (this->m_extentOnSheet.height * blooDot::Consts::SQUARE_HEIGHT) / 2.f;
	this->m_Bounding->OuterRim = D2D1::RectF(midPointX - this->outerRadius, midPointY - this->outerRadius, midPointX + this->outerRadius, midPointY + this->outerRadius);
	BoundingGeometry circularBounds;
	circularBounds.Shape = BoundingGeometry::Primitive::Circle;
	circularBounds.Bounds = D2D1_RECT_F(this->m_Bounding->OuterRim);
	this->m_Bounding->Geometries.push_back(circularBounds);
}

void FlameGhost::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, const D2D1_RECT_F *rect)
{
	MFARGB colitor{ 11, 137, 221, 255 };
	MFARGB colring{ 12, 12, 12, 255 };
	MFARGB colwedg{ 240, 242, 237, 255 };
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl;
	Microsoft::WRL::ComPtr<ID2D1Brush> brushWedge;
	D2D1_ELLIPSE elli = D2D1::Ellipse(D2D1::Point2F(rect->left + (rect->right - rect->left) / 2.0f, rect->top + (rect->bottom - rect->top) / 2.0f), this->rimRadius, this->rimRadius);
	brusherl = m_Brushes->WannaHave(drawTo, colitor);
	brushWedge = m_Brushes->WannaHave(drawTo, colwedg);
	auto rimBrush = brusherl.Get();
	auto wedgeBrush = brushWedge.Get();
	auto rendEr = drawTo.Get();
	rendEr->FillEllipse(elli, rimBrush);
	
	/* flame spikes */
	auto twoPi = static_cast<float>(M_PI) * 2.f;
	auto piStep = twoPi / 11.f;
	auto anglePadding = piStep / 5.f;	
	for (int spikeIndex = 0; spikeIndex < 11; ++spikeIndex)
	{
		auto roundRobin = spikeIndex * piStep;
		auto startpointAngle = roundRobin + anglePadding;
		auto midpointAngle = roundRobin + (piStep / 2.f);
		auto endpointAngle = roundRobin + piStep - anglePadding;

		Microsoft::WRL::ComPtr<ID2D1PathGeometry> spikeSegments;
		Microsoft::WRL::ComPtr<ID2D1GeometrySink> geometrySink;
		DX::ThrowIfFailed
		(
			this->m_deviceResources->GetD2DFactory()->CreatePathGeometry(spikeSegments.GetAddressOf())
		);

		DX::ThrowIfFailed
		(
			spikeSegments->Open(geometrySink.GetAddressOf())
		);

		geometrySink->BeginFigure(
			D2D1::Point2F(elli.point.x + innerRadius * cosf(startpointAngle), elli.point.y + this->innerRadius * sinf(startpointAngle)),
			D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED
		);

		geometrySink->AddLine(D2D1::Point2F(elli.point.x + this->outerRadius * cosf(midpointAngle), elli.point.y + this->outerRadius * sinf(midpointAngle)));
		geometrySink->AddLine(D2D1::Point2F(elli.point.x + this->innerRadius * cosf(endpointAngle), elli.point.y + this->innerRadius * sinf(endpointAngle)));
		geometrySink->AddArc(D2D1::ArcSegment(
			D2D1::Point2F(elli.point.x + this->innerRadius * cosf(startpointAngle), elli.point.y + this->innerRadius * sinf(startpointAngle)),
			D2D1::SizeF(this->innerRadius, this->innerRadius),
			0.f,
			D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
			D2D1_ARC_SIZE::D2D1_ARC_SIZE_SMALL
		));

		geometrySink->EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED);
		DX::ThrowIfFailed
		(
			geometrySink->Close()
		);

		rendEr->FillGeometry(spikeSegments.Get(), wedgeBrush);
		rendEr->DrawGeometry(spikeSegments.Get(), rimBrush, Mob::WEDGE_STROKE);
		rendEr->DrawLine(
			D2D1::Point2F(0, 0),
			D2D1::Point2F(0, 0),
			rimBrush,
			Mob::HAIRLINE
		);
	}

	/* interior last */
	brusherl = m_Brushes->WannaHave(drawTo, colring);
	auto ringBrush = brusherl.Get();
	elli.radiusX = elli.radiusY = (elli.radiusX - 2.5f);
	rendEr->DrawEllipse(elli, ringBrush, 0.9f);

	/* la bocca */
	rendEr->DrawLine(
		D2D1::Point2F(elli.point.x - 13.f, elli.point.y + 5.f),
		D2D1::Point2F(elli.point.x + 13.f, elli.point.y + 5.f),
		ringBrush, 1.5f
	);

	rendEr->DrawLine(
		D2D1::Point2F(elli.point.x + 13.f, elli.point.y + 5.f),
		D2D1::Point2F(elli.point.x, elli.point.y + 23.5f),
		ringBrush, 1.5f
	);

	rendEr->DrawLine(
		D2D1::Point2F(elli.point.x, elli.point.y + 23.5f),
		D2D1::Point2F(elli.point.x - 13.f, elli.point.y + 5.f),
		ringBrush, 1.5f
	);

	/* left eye */
	rendEr->DrawLine(
		D2D1::Point2F(elli.point.x - 27.f, elli.point.y - 5.f),
		D2D1::Point2F(elli.point.x - 5.f, elli.point.y - 5.f),
		ringBrush, 1.5f
	);

	rendEr->DrawLine(
		D2D1::Point2F(elli.point.x - 5.f, elli.point.y - 5.f),
		D2D1::Point2F(elli.point.x - 16.f, elli.point.y - 25.f),
		ringBrush, 1.5f
	);

	rendEr->DrawLine(
		D2D1::Point2F(elli.point.x - 16.f, elli.point.y - 25.f),
		D2D1::Point2F(elli.point.x - 27.f, elli.point.y - 5.f),
		ringBrush, 1.5f
	);

	/* rheye */
	rendEr->DrawLine(
		D2D1::Point2F(elli.point.x + 5.f, elli.point.y - 5.f),
		D2D1::Point2F(elli.point.x + 27.f, elli.point.y - 5.f),
		ringBrush, 1.5f
	);

	rendEr->DrawLine(
		D2D1::Point2F(elli.point.x + 27.f, elli.point.y - 5.f),
		D2D1::Point2F(elli.point.x + 16.f, elli.point.y - 25.f),
		ringBrush, 1.5f
	);

	rendEr->DrawLine(
		D2D1::Point2F(elli.point.x + 16.f, elli.point.y - 25.f),
		D2D1::Point2F(elli.point.x + 5.f, elli.point.y - 5.f),
		ringBrush, 1.5f
	);
}
