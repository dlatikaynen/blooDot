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
	this->m_lookupShy.x = canvasX;
	this->m_lookupShy.y = canvasY;
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
	rect.left = 50.0f * this->m_lookupShy.x;
	rect.top = 50.0f * this->m_lookupShy.y;
	rect.right = rect.left + 49;
	rect.bottom = rect.top + 49;
	drawTo->FillRectangle(rect, brrect.Get());


}

void Dings::SetSheetPlacementsFromCoalescability()
{
	if (this->m_Coalescing != Facings::Shy) 
	{
		//this->m_sheetPlacementFC.x = m_sheetPlacement.x + 1;
		//this->m_sheetPlacementFC.y = m_sheetPlacement.x + 0;
	}
}

D2D1_POINT_2U Dings::GetSheetPlacement(Facings coalesced)
{
	switch (coalesced)
	{
		case Facings::Shy: break;
		case Facings::Immersed: return this->m_lookupImmersed;
		case Facings::Center: return this->m_lookupCrossing;
		case Facings::NS: return this->m_lookupPipes[OrientabilityIndexDuplex::Vertically];
		case Facings::WE: return this->m_lookupPipes[OrientabilityIndexDuplex::Horizontally];
		case Facings::West: return this->m_lookupU[OrientabilityIndexQuadruplet::Lefty];
		case Facings::East: return this->m_lookupU[OrientabilityIndexQuadruplet::Righty];
		case Facings::North: return this->m_lookupU[OrientabilityIndexQuadruplet::Uppy];
		case Facings::South: return this->m_lookupU[OrientabilityIndexQuadruplet::Downy];
		case Facings::NW: return this->m_lookupEdges[OrientabilityIndexDiagon::DiagNW];
		case Facings::SW: return this->m_lookupEdges[OrientabilityIndexDiagon::DiagSW];
		case Facings::NE: return this->m_lookupEdges[OrientabilityIndexDiagon::DiagNE];
		case Facings::SE: return this->m_lookupEdges[OrientabilityIndexDiagon::DiagSE];
		case Facings::EdgeW: return this->m_lookupSides[OrientabilityIndexQuadruplet::Lefty];
		case Facings::EdgeN: return this->m_lookupSides[OrientabilityIndexQuadruplet::Uppy];
		case Facings::EdgeE: return this->m_lookupSides[OrientabilityIndexQuadruplet::Righty];
		case Facings::EdgeS: return this->m_lookupSides[OrientabilityIndexQuadruplet::Downy];
		case Facings::EdgeNW: return this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagNW];
		case Facings::EdgeSW: return this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagSW];
		case Facings::EdgeNE: return this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagNE];
		case Facings::EdgeSE: return this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagSE];
		case Facings::EdgeCornerWD: return this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Lefty];
		case Facings::EdgeCornerWU: return this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::LeftyFar];
		case Facings::EdgeCornerED: return this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Righty];
		case Facings::EdgeCornerEU: return this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::RightyFar];
		case Facings::EdgeCornerSL: return this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Downy];
		case Facings::EdgeCornerSR: return this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::DownyFar];
		case Facings::EdgeCornerNL: return this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Uppy];
		case Facings::EdgeCornerNR: return this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::UppyFar];
		case Facings::EdgeCornerNW: return this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagNW];
		case Facings::EdgeCornerSE: return this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagSE];
		case Facings::EdgeCornerNE: return this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagNE];
		case Facings::EdgeCornerSW: return this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagSW];
		case Facings::TW: return this->m_lookupTs[OrientabilityIndexQuadruplet::Lefty];
		case Facings::TE: return this->m_lookupTs[OrientabilityIndexQuadruplet::Righty];
		case Facings::TN: return this->m_lookupTs[OrientabilityIndexQuadruplet::Uppy];
		case Facings::TS: return this->m_lookupTs[OrientabilityIndexQuadruplet::Downy];
		case Facings::Corner1NW: return this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagNW];
		case Facings::Corner1NE: return this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagNE];
		case Facings::Corner1SW: return this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagSW];
		case Facings::Corner1SE: return this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagSE];
		case Facings::Corner2W: return this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Lefty];
		case Facings::Corner2E: return this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Righty];
		case Facings::Corner2S: return this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Downy];
		case Facings::Corner2N: return this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Uppy];
		case Facings::Corner3NW: return this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagNW];
		case Facings::Corner3NE: return this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagNE];
		case Facings::Corner3SW: return this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagSW];
		case Facings::Corner3SE: return this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagSE];
	}

	return this->m_lookupShy;
}
