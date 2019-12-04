#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

Coin::Coin(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(6, "Coin", deviceResources, drawBrushes)
{
	m_Facings = Facings::Shy;
	m_Coalescing = Facings::Shy;
	m_preferredLayer = Layers::Walls;
	m_possibleLayers = Layers::Walls;
}

void Coin::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	MFARGB colrect { 10, 219, 221, 255 };
	MFARGB colring{ 12, 12, 12, 255 };
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl;
	PrepareRect(&this->m_lookupShy, rect);
	D2D1_ELLIPSE elli = D2D1::Ellipse(D2D1::Point2F(rect.left + (rect.right - rect.left) / 2.0f, rect.top + (rect.bottom - rect.top) / 2.0f), 10.0f, 10.0f);
	brusherl = m_Brushes->WannaHave(drawTo, colrect);
	auto innerBrush = brusherl.Get();
	auto rendEr = drawTo.Get();
	rendEr->FillEllipse(elli, innerBrush);
	brusherl = m_Brushes->WannaHave(drawTo, colring);
	auto ringBrush = brusherl.Get();
	elli.radiusX = elli.radiusY = (elli.radiusX - 2.5f);
	rendEr->DrawEllipse(elli, ringBrush, 0.9f);
	rendEr->DrawLine(D2D1::Point2F(elli.point.x + 0.5f, elli.point.y - 5.0f), D2D1::Point2F(elli.point.x + 0.5f, elli.point.y + 5.0f), ringBrush, 1.5f);
	rendEr->DrawLine(D2D1::Point2F(elli.point.x - 2.5f, elli.point.y - 3.0f), D2D1::Point2F(elli.point.x + 0.5f, elli.point.y - 5.0f), ringBrush, 1.5f);
}

Chest::Chest(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(7, "Chest", deviceResources, drawBrushes)
{
	m_Facings = Facings::Viech;
	m_Coalescing = Facings::Shy;
	m_preferredLayer = Layers::Walls;
	m_possibleLayers = Layers::Walls;
}

// the lefty representation (assuming placement against a wall)
void Chest::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	/* declare */
	D2D1_RECT_F rect;		
	/* prepare */
	PrepareRect(&this->m_lookupShy, rect);
	auto rendEr = drawTo.Get();
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl = m_Brushes->WannaHave(drawTo, MFARGB{ 0, 102, 153, 255 });
	auto brushBase = brusherl.Get();
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl2 = m_Brushes->WannaHave(drawTo, MFARGB{ 0, 91, 143, 255 });
	auto brushDark = brusherl2.Get();
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl3 = m_Brushes->WannaHave(drawTo, MFARGB{ 51, 102, 102, 255 });
	auto brushBark = brusherl3.Get();
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl4 = m_Brushes->WannaHave(drawTo, MFARGB{ 121, 132, 132, 255 });
	auto brushNail = brusherl4.Get();
	/* draw away */
	for (int facing = OrientabilityIndexQuadruplet::Uppy; facing >= OrientabilityIndexQuadruplet::Lefty; --facing)
	{
		PrepareRect(&this->m_lookupSides[facing], rect);
		Rotate(rendEr, rect, facing);
		auto left = rect.left + 1.0f;
		auto rite = rect.left + ((rect.right - rect.left) - 4.0f) * blooDot::Consts::GOLDEN_RATIO;
		auto yctr = rect.top + (rect.bottom - rect.top) / 2.0f;
		auto step = (rite - left) / 4.0f;
		rendEr->FillRectangle(D2D1::RectF(left, rect.top + 2.0f, rite, rect.bottom - 2.0f), brushBase);
		rendEr->DrawLine(D2D1::Point2F(rect.left + step, rect.top + 2.0f), D2D1::Point2F(rect.left + step, rect.bottom - 2.0f), brushDark);
		rendEr->DrawLine(D2D1::Point2F(rect.left + step * 2.0f, rect.top + 2.0f), D2D1::Point2F(rect.left + step * 2.0f, rect.bottom - 2.0f), brushDark);
		rendEr->DrawLine(D2D1::Point2F(rect.left + step * 3.0f, rect.top + 2.0f), D2D1::Point2F(rect.left + step * 3.0f, rect.bottom - 2.0f), brushDark);
		rendEr->FillRectangle(D2D1::RectF(left, rect.top + 3.0f, rite, rect.top + 10.0f), brushBark);
		rendEr->FillRectangle(D2D1::RectF(left, rect.bottom - 10.0f, rite, rect.bottom - 3.0f), brushBark);
		auto elli = D2D1::Ellipse(D2D1::Point2F(rect.left + step, rect.top + 6.5f), 1.3f, 1.3f);
		rendEr->FillEllipse(elli, brushNail);
		elli = D2D1::Ellipse(D2D1::Point2F(rect.left + step * 2.0f, rect.top + 6.5f), 1.3f, 1.3f);
		rendEr->FillEllipse(elli, brushNail);
		elli = D2D1::Ellipse(D2D1::Point2F(rect.left + step * 3.0f, rect.top + 6.5f), 1.3f, 1.3f);
		rendEr->FillEllipse(elli, brushNail);
		elli = D2D1::Ellipse(D2D1::Point2F(rect.left + step, rect.bottom - 6.5f), 1.3f, 1.3f);
		rendEr->FillEllipse(elli, brushNail);
		elli = D2D1::Ellipse(D2D1::Point2F(rect.left + step * 2.0f, rect.bottom - 6.5f), 1.3f, 1.3f);
		rendEr->FillEllipse(elli, brushNail);
		elli = D2D1::Ellipse(D2D1::Point2F(rect.left + step * 3.0f, rect.bottom - 6.5f), 1.3f, 1.3f);
		rendEr->FillEllipse(elli, brushNail);
		rendEr->FillRectangle(D2D1::RectF(rite, yctr - 1.8f, rite + 1.5f, yctr + 1.8f), brushNail);
	}
}