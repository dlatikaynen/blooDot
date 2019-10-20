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
	this->m_sheetPlacement.x = canvasX;
	this->m_sheetPlacement.y = canvasY;
	this->SetSheetPlacementsFromCoalescability();
	this->DrawInternal(drawTo);
}

void Dings::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	MFARGB colrect;
	colrect.rgbAlpha = 255;
	colrect.rgbRed = 0;
	colrect.rgbGreen = 0;
	colrect.rgbBlue = 0;
	
	D2D1_RECT_F rect;
	Microsoft::WRL::ComPtr<ID2D1Brush> brrect = m_Brushes.WannaHave(drawTo, colrect);
	rect.left = 50.0f * this->m_sheetPlacement.x;
	rect.top = 50.0f * this->m_sheetPlacement.y;
	rect.right = rect.left + 49;
	rect.bottom = rect.top + 49;
	drawTo->FillRectangle(rect, brrect.Get());


}

void Dings::SetSheetPlacementsFromCoalescability()
{
	if (this->m_Coalescing != Facings::Shy) 
	{
		this->m_sheetPlacementFC.x = m_sheetPlacement.x + 1;
		this->m_sheetPlacementFC.y = m_sheetPlacement.x + 0;
	}
}

D2D1_POINT_2U Dings::GetSheetPlacement(Facings coalesced)
{
	switch (coalesced)
	{
		case Facings::Shy:	break;
		case Facings::East: return this->m_sheetPlacementCE;
	}

	return this->m_sheetPlacement;
}
