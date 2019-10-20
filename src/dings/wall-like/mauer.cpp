#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

Mauer::Mauer(BrushRegistry drawBrushes) : Dings(1, "Mauer", drawBrushes) { }

void Mauer::Draw(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, int canvasX, int canvasY)
{
	//Ding::Draw();
	MFARGB colrect;
	colrect.rgbAlpha = 255;
	colrect.rgbRed = 192;
	colrect.rgbGreen = 192;
	colrect.rgbBlue = 192;

	D2D1_RECT_F rect;
	Microsoft::WRL::ComPtr<ID2D1Brush> brrect = m_Brushes.WannaHave(drawTo, colrect);
	rect.left = 50.0f * canvasX;
	rect.top = 50.0f * canvasY;
	rect.right = rect.left + 49;
	rect.bottom = rect.top + 49;
	drawTo->FillRectangle(rect, brrect.Get());
}