#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

Mauer::Mauer(BrushRegistry drawBrushes) : Dings(1, "Mauer", drawBrushes) 
{
	m_Facings = Facings::Shy;
	m_Coalescing = Facings::Immersed;
	m_preferredLayer = Layers::Walls;
	m_possibleLayers = Layers::Walls;
}

void Mauer::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	MFARGB colrect;
	Microsoft::WRL::ComPtr<ID2D1Brush> brrect;
	auto rendEr = drawTo.Get();

	/* SHY
	 * inner solid */
	colrect = { 128, 128, 128, 255 };
	brrect = m_Brushes.WannaHave(drawTo, colrect);

	rect.left = 49.0f * this->m_lookupShy.x;
	rect.top = 49.0f * this->m_lookupShy.y;
	rect.right = rect.left + 10 * 49;
	rect.bottom = rect.top + 10 * 49;
	rendEr->FillRectangle(rect, brrect.Get());

	/* IMMERSED - implied */
	colrect = { 192, 192, 192, 192 };
	brrect = m_Brushes.WannaHave(drawTo, colrect);
	auto innerBrush = brrect.Get();

	/* CENTER CROSSING */
	rect.left = 49.0f * this->m_lookupCrossing.x;
	rect.top = 49.0f * this->m_lookupCrossing.y;
	rect.right = rect.left + 49.0;
	rect.bottom = rect.top + 49.0;	
	rendEr->DrawLine(D2D1::Point2F(rect.left, rect.top+5), D2D1::Point2F(rect.left+5, rect.top+5), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.left+5, rect.top+5), D2D1::Point2F(rect.left+5, rect.top), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top + 5), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.left, rect.bottom - 5), D2D1::Point2F(rect.left + 5, rect.bottom - 5), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.bottom - 5), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.right, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom - 5), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom), innerBrush, 2.5f, 0);
	
	/* SHY
	 * inner border */
	rect.left = 49.0f * this->m_lookupShy.x + 5;
	rect.top = 49.0f * this->m_lookupShy.y + 5;
	rect.right = rect.left + 49.0 - 10;
	rect.bottom = rect.top + 49.0 - 10;
	rendEr->DrawRectangle(rect, innerBrush, 2.5f, 0);

	/* Us: rotate so we have lefty (no rotation) last, so
	 * after the loop ends the context is rotation-free */
	for (int facing = OrientabilityIndexQuadruplet::Uppy; facing >= OrientabilityIndexQuadruplet::Lefty; --facing)
	{
		PrepareRect(&this->m_lookupU[facing], rect);
		Rotate(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.right, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.bottom - 5), D2D1::Point2F(rect.right, rect.bottom - 5), innerBrush, 2.5f, 0);
		/* Ts */
		PrepareRect(&this->m_lookupTs[facing], rect);
		Rotate(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom), innerBrush, 2.5f, 0);
		/* single outer edges (sides) */
		PrepareRect(&this->m_lookupSides[facing], rect);
		Rotate(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
		/* double corners */
		PrepareRect(&this->m_lookupCornersInner2[facing], rect);
		Rotate(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom), innerBrush, 2.5f, 0);
	}

	/* pipes and double inner corners (dually oriented specialities) */
	for (int facing = OrientabilityIndexDuplex::Vertically; facing >= OrientabilityIndexDuplex::Horizontally; --facing)
	{
		PrepareRect(&this->m_lookupPipes[facing], rect);
		Rotate(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left, rect.top + 5), D2D1::Point2F(rect.right, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left, rect.bottom - 5), D2D1::Point2F(rect.right, rect.bottom - 5), innerBrush, 2.5f, 0);
		PrepareRect(&this->m_lookupCornersDiag[facing], rect);
		Rotate(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left, rect.bottom - 5), D2D1::Point2F(rect.left + 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.bottom - 5), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
	}

	for (int facing = OrientabilityIndexDiagon::DiagNE; facing >= OrientabilityIndexDiagon::DiagNW; --facing)
	{
		/* outer-only edges (diagonal) */
		PrepareRect(&this->m_lookupEdgesOuter90[facing], rect);
		Rotate(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.right, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
		/* genuine edges (knees) */
		PrepareRect(&this->m_lookupEdges[facing], rect);
		Rotate(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.right, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom), innerBrush, 2.5f, 0);
		/* edges with one inner corner, near */
		PrepareRect(&this->m_lookupEdgesInner1[facing], rect);
		Rotate(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top), innerBrush, 2.5f, 0);
		/* edges with one inner corner, far */
		PrepareRect(&this->m_lookupEdgesInner1[facing + FAR_OFFSET], rect);
		Rotate(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom), innerBrush, 2.5f, 0);
		/* single corners */
		PrepareRect(&this->m_lookupCornersInner1[facing], rect);
		Rotate(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top), innerBrush, 2.5f, 0);
		/* triple corners */
		PrepareRect(&this->m_lookupCornersInner3[facing], rect);
		Rotate(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.top), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left, rect.bottom - 5), D2D1::Point2F(rect.left + 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.bottom - 5), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
	}
}