#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

Dalek::Dalek(BrushRegistry drawBrushes) : Dings(1, "Dalek", drawBrushes)
{
	m_Facings = Facings::Viech;
	m_Coalescing = Facings::Shy;
	m_preferredLayer = Layers::Walls;
	m_possibleLayers = Layers::Walls;
}

void Dalek::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	MFARGB colrect;
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl;
	auto rendEr = drawTo.Get();

	/* IMMERSED - implied */
	colrect = { 192, 192, 192, 64 };
	brusherl = m_Brushes.WannaHave(drawTo, colrect);
	auto innerBrush = brusherl.Get();

	/* lefty (no rotation) last, so after the loop ends the context is rotation-free */
	for (int facing = OrientabilityIndexQuadruplet::Uppy; facing >= OrientabilityIndexQuadruplet::Lefty; --facing)
	{
		PrepareRect(&this->m_lookupSides[facing], rect);
		Rotate(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.right, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.bottom - 5), D2D1::Point2F(rect.right, rect.bottom - 5), innerBrush, 2.5f, 0);

	}	
}