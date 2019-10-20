#include "..\PreCompiledHeaders.h"
#include "dings.h"

Dings::Dings(int dingID, Platform::String^ dingName, BrushRegistry drawBrushes)
{
	this->m_ID = dingID;
	this->m_Name = dingName;
	this->m_Brushes = drawBrushes;
}

int Dings::ID() 
{
	return this->m_ID;
}

Platform::String^ Dings::Name() 
{
	return this->m_Name;
}

void Dings::Draw(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, int canvasX, int canvasY)
{
	MFARGB colrect;
	colrect.rgbAlpha = 255;
	colrect.rgbRed = 0;
	colrect.rgbGreen = 0;
	colrect.rgbBlue = 0;
	
	D2D1_RECT_F rect;
	Microsoft::WRL::ComPtr<ID2D1Brush> brrect = m_Brushes.WannaHave(drawTo, colrect);
	rect.left = 50.0f * canvasX;
	rect.top = 50.0f * canvasY;
	rect.right = rect.left + 49;
	rect.bottom = rect.top + 49;
	drawTo->FillRectangle(rect, brrect.Get());


}


