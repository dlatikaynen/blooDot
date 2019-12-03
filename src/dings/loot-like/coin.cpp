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