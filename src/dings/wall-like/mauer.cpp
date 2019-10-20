#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

Mauer::Mauer(BrushRegistry drawBrushes) : Dings(1, "Mauer", drawBrushes) 
{
	m_Facing = Facings::Shy;
	m_Coalescing = Facings::Immersed;
	m_preferredLayer = Layers::Walls;
	m_possibleLayers = Layers::Walls;
}

void Mauer::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	MFARGB colrect;
	Microsoft::WRL::ComPtr<ID2D1Brush> brrect;

	/* SHY
	 * inner solid */
	colrect = { 128, 128, 128, 255 };
	brrect = m_Brushes.WannaHave(drawTo, colrect);

	rect.left = 49.0f * this->m_lookupShy.x;
	rect.top = 49.0f * this->m_lookupShy.y;
	rect.right = rect.left + 8 * 49;
	rect.bottom = rect.top + 8 * 49;
	drawTo->FillRectangle(rect, brrect.Get());

	/* IMMERSED - implied */

	colrect = { 192, 192, 192, 192 };
	brrect = m_Brushes.WannaHave(drawTo, colrect);
	auto innerBrush = brrect.Get();

	/* CENTER CROSSING */
	rect.left = 49.0f * this->m_lookupCrossing.x;
	rect.top = 49.0f * this->m_lookupCrossing.y;
	rect.right = rect.left + 49.0;
	rect.bottom = rect.top + 49.0;
	drawTo->DrawLine(D2D1::Point2F(rect.left, rect.top+5), D2D1::Point2F(rect.left+5, rect.top+5), innerBrush, 2.5f, 0);
	drawTo->DrawLine(D2D1::Point2F(rect.left+5, rect.top+5), D2D1::Point2F(rect.left+5, rect.top), innerBrush, 2.5f, 0);

	drawTo->DrawLine(D2D1::Point2F(rect.left, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.top + 5), innerBrush, 2.5f, 0);
	drawTo->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.top), innerBrush, 2.5f, 0);

	drawTo->DrawLine(D2D1::Point2F(rect.left, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.top + 5), innerBrush, 2.5f, 0);
	drawTo->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.top), innerBrush, 2.5f, 0);

	drawTo->DrawLine(D2D1::Point2F(rect.left, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.top + 5), innerBrush, 2.5f, 0);
	drawTo->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.top), innerBrush, 2.5f, 0);

	/* SHY
	 * inner border */

	rect.left = 49.0f * this->m_lookupShy.x + 5;
	rect.top = 49.0f * this->m_lookupShy.y + 5;
	rect.right = rect.left + 49.0 - 10;
	rect.bottom = rect.top + 49.0 - 10;
	drawTo->DrawRectangle(rect, innerBrush, 2.5f, 0);


}