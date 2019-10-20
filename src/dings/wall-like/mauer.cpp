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

	rect.left = 50.0f * this->m_lookupShy.x;
	rect.top = 50.0f * this->m_lookupShy.y;
	rect.right = rect.left + 49;
	rect.bottom = rect.top + 49;
	drawTo->FillRectangle(rect, brrect.Get());

	/* inner border */
	colrect = { 192, 192, 192, 192};
	brrect = m_Brushes.WannaHave(drawTo, colrect);

	rect.left += 6;
	rect.top += 6;
	rect.right -= 5;
	rect.bottom -= 5;
	drawTo->DrawRectangle(rect, brrect.Get(), 2.5f, 0);


}