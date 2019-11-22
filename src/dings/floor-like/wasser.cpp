#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

Wasser::Wasser(BrushRegistry* drawBrushes) : Dings(2, "Wasser", drawBrushes)
{
	m_Facings = Facings::Shy;
	m_Coalescing = Facings::Shy;
	m_preferredLayer = Layers::Floor;
	m_possibleLayers = Layers::Floor;
}

void Wasser::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	MFARGB colrect { 192, 0, 0, 255 };
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl;
	PrepareRect(&this->m_lookupShy, rect);
	brusherl = m_Brushes->WannaHave(drawTo, colrect);
	auto innerBrush = brusherl.Get();
	auto rendEr = drawTo.Get();
	rendEr->FillRectangle(rect, innerBrush);
}