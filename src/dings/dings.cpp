#include "..\PreCompiledHeaders.h"
#include "dings.h"

using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;
using namespace std;

Facings& operator |=(Facings& a, Facings b)
{
	return a = a | b;
}

Dings::Dings(int dingID, Platform::String^ dingName, std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes)
{
	this->m_ID = dingID;
	this->m_Name = dingName;
	this->m_deviceResources = deviceResources;
	this->m_Brushes = drawBrushes;
}

unsigned Dings::ID() 
{
	return this->m_ID;
}

Platform::String^ Dings::Name() 
{
	return this->m_Name;
}

Layers Dings::GetPreferredLayer()
{
	return this->m_preferredLayer;
}

void Dings::Draw(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, int canvasX, int canvasY)
{
	this->m_lookupShy.x = canvasX;
	this->m_lookupShy.y = canvasY;
	this->SetSheetPlacementsFromCoalescability();
	this->m_fromFile = this->ShouldLoadFromBitmap();
	if (this->m_fromFile == nullptr)
	{
		this->DrawInternal(drawTo);
	}
	else
	{
		auto bitMap = this->LoadFromBitmap();
		auto rect = D2D1::Rect(
			49.0f * this->m_lookupShy.x,
			49.0f * this->m_lookupShy.y,
			49.0f * this->m_lookupShy.x + 49.0f,
			49.0f * this->m_lookupShy.y + 49.0f
		);

		drawTo->DrawBitmap(bitMap.Get(), rect);
		bitMap.Reset();
	}
}

Platform::String^ Dings::ShouldLoadFromBitmap()
{
	return nullptr;
}

Microsoft::WRL::ComPtr<ID2D1Bitmap> Dings::LoadFromBitmap()
{	
	Microsoft::WRL::ComPtr<ID2D1Bitmap>	bitMap;
	auto loader = ref new BasicLoader(this->m_deviceResources->GetD3DDevice());
	auto fullPath = Platform::String::Concat(L"Media\\Bitmaps\\", this->m_fromFile);
	loader->LoadPngToBitmap(fullPath, this->m_deviceResources, &bitMap);
	return bitMap;
}

void Dings::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	MFARGB colrect;
	colrect.rgbAlpha = 255;
	colrect.rgbRed = 0;
	colrect.rgbGreen = 0;
	colrect.rgbBlue = 0;
	
	D2D1_RECT_F rect;
	Microsoft::WRL::ComPtr<ID2D1Brush> brrect = m_Brushes->WannaHave(drawTo, colrect);
	rect.left = 49.0f * this->m_lookupShy.x;
	rect.top = 49.0f * this->m_lookupShy.y;
	rect.right = rect.left + 49;
	rect.bottom = rect.top + 49;
	drawTo->FillRectangle(rect, brrect.Get());
}

void Dings::SetSheetPlacementsFromCoalescability()
{
	int x = this->m_lookupShy.x;
	int y = this->m_lookupShy.y;

	if (this->m_Facings == Facings::Viech)
	{
		this->m_lookupSides[OrientabilityIndexQuadruplet::Lefty].x = x++;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Lefty].y = y;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Uppy].x = x++;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Uppy].y = y;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Righty].x = x++;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Righty].y = y;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Downy].x = x++;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Downy].y = y;
	}
	else if (this->m_Coalescing != Facings::Shy) 
	{		
		this->m_lookupImmersed.x = ++x;
		this->m_lookupImmersed.y = y;
		this->m_lookupCrossing.x = ++x;
		this->m_lookupCrossing.y = y;
		this->m_lookupPipes[OrientabilityIndexDuplex::Vertically].x = ++x;
		this->m_lookupPipes[OrientabilityIndexDuplex::Vertically].y = y;
		this->m_lookupPipes[OrientabilityIndexDuplex::Horizontally].x = ++x;
		this->m_lookupPipes[OrientabilityIndexDuplex::Horizontally].y = y;
		this->m_lookupU[OrientabilityIndexQuadruplet::Lefty].x = ++x;
		this->m_lookupU[OrientabilityIndexQuadruplet::Lefty].y = y;
		this->m_lookupU[OrientabilityIndexQuadruplet::Righty].x = ++x;
		this->m_lookupU[OrientabilityIndexQuadruplet::Righty].y = y;
		this->m_lookupU[OrientabilityIndexQuadruplet::Uppy].x = ++x;
		this->m_lookupU[OrientabilityIndexQuadruplet::Uppy].y = y;
		this->m_lookupU[OrientabilityIndexQuadruplet::Downy].x = ++x;
		this->m_lookupU[OrientabilityIndexQuadruplet::Downy].y = y;

		x = this->m_lookupShy.x;
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagNW].x = x;
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagNW].y = ++y;
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagSW].x = ++x;
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagSW].y = y;
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagNE].x = ++x;
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagNE].y = y;
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagSE].x = ++x;
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagSE].y = y;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Lefty].x = ++x;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Lefty].y = y;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Uppy].x = ++x;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Uppy].y = y;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Righty].x = ++x;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Righty].y = y;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Downy].x = ++x;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Downy].y = y;

		x = this->m_lookupShy.x;
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagNW].x = x;
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagNW].y = ++y;
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagSW].x = ++x;
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagSW].y = y;
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagNE].x = ++x;
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagNE].y = y;
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagSE].x = ++x;
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagSE].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Lefty].x = ++x;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Lefty].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::LeftyFar].x = ++x;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::LeftyFar].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Righty].x = ++x;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Righty].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::RightyFar].x = ++x;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::RightyFar].y = y;

		x = this->m_lookupShy.x;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Downy].x = x;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Downy].y = ++y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::DownyFar].x = ++x;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::DownyFar].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Uppy].x = ++x;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Uppy].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::UppyFar].x = ++x;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::UppyFar].y = y;
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagNW].x = ++x;
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagNW].y = y;
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagSE].x = ++x;
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagSE].y = y;
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagNE].x = ++x;
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagNE].y = y;
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagSW].x = ++x;
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagSW].y = y;

		x = this->m_lookupShy.x;
		this->m_lookupTs[OrientabilityIndexQuadruplet::Lefty].x = x;
		this->m_lookupTs[OrientabilityIndexQuadruplet::Lefty].y = ++y;
		this->m_lookupTs[OrientabilityIndexQuadruplet::Righty].x = ++x;
		this->m_lookupTs[OrientabilityIndexQuadruplet::Righty].y = y;
		this->m_lookupTs[OrientabilityIndexQuadruplet::Uppy].x = ++x;
		this->m_lookupTs[OrientabilityIndexQuadruplet::Uppy].y = y;
		this->m_lookupTs[OrientabilityIndexQuadruplet::Downy].x = ++x;
		this->m_lookupTs[OrientabilityIndexQuadruplet::Downy].y = y;
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagNW].x = ++x;
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagNW].y = y;
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagNE].x = ++x;
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagNE].y = y;
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagSW].x = ++x;
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagSW].y = y;
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagSE].x = ++x;
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagSE].y = y;

		x = this->m_lookupShy.x;
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Lefty].x = x;
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Lefty].y = ++y;
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Righty].x = ++x;
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Righty].y = y;
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Downy].x = ++x;
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Downy].y = y;
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Uppy].x = ++x;
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Uppy].y = y;
		this->m_lookupCornersDiag[OrientabilityIndexDiagon::DiagNW].x = ++x;
		this->m_lookupCornersDiag[OrientabilityIndexDiagon::DiagNW].y = y;
		this->m_lookupCornersDiag[OrientabilityIndexDiagon::DiagSW].x = ++x;
		this->m_lookupCornersDiag[OrientabilityIndexDiagon::DiagSW].y = y;
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagNW].x = ++x;
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagNW].y = y;
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagNE].x = ++x;
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagNE].y = y;

		x = this->m_lookupShy.x;
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagSW].x = x;
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagSW].y = ++y;
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagSE].x = ++x;
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagSE].y = y;
	}
}

void Dings::PrepareRect(D2D1_POINT_2U *lookupLocation, D2D1_RECT_F &rectToSet)
{
	rectToSet.left = 49.0f * lookupLocation->x;
	rectToSet.top = 49.0f * lookupLocation->y;
	rectToSet.right = rectToSet.left + 49.0f;
	rectToSet.bottom = rectToSet.top + 49.0f;
}

void Dings::Rotate(ID2D1RenderTarget *rendEr, D2D1_RECT_F rect, int rotation)
{
	if (rotation == OrientabilityIndexQuadruplet::Lefty)
	{
		rendEr->SetTransform(D2D1::Matrix3x2F::Identity());
	}
	else
	{
		/* take anything else but 90° and funny stuff happens */
		rendEr->SetTransform(D2D1::Matrix3x2F::Rotation(static_cast<float>(rotation * -90), D2D1::Point2F(rect.left + 24.5f, rect.top + 24.5f)));
	}
}

D2D1_POINT_2U Dings::GetSheetPlacement(Facings orientation)
{
	if (this->m_Facings == Facings::Viech)
	{
		switch (orientation)
		{
			case Facings::West: return this->m_lookupSides[OrientabilityIndexQuadruplet::Lefty];
			case Facings::North:return this->m_lookupSides[OrientabilityIndexQuadruplet::Uppy];
			case Facings::East: return this->m_lookupSides[OrientabilityIndexQuadruplet::Righty];
			case Facings::South: return this->m_lookupSides[OrientabilityIndexQuadruplet::Downy];
		}
	}
	else if (this->m_Facings == Facings::Shy && this->m_Coalescing == Facings::Immersed)
	{
		switch (orientation)
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
			case Facings::TW: return this->m_lookupTs[OrientabilityIndexQuadruplet::Righty];
			case Facings::TE: return this->m_lookupTs[OrientabilityIndexQuadruplet::Lefty];
			case Facings::TN: return this->m_lookupTs[OrientabilityIndexQuadruplet::Downy];
			case Facings::TS: return this->m_lookupTs[OrientabilityIndexQuadruplet::Uppy];
			case Facings::Corner1NW: return this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagNW];
			case Facings::Corner1NE: return this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagNE];
			case Facings::Corner1SW: return this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagSW];
			case Facings::Corner1SE: return this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagSE];
			case Facings::Corner2W: return this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Lefty];
			case Facings::Corner2E: return this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Righty];
			case Facings::Corner2S: return this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Downy];
			case Facings::Corner2N: return this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Uppy];
			case Facings::CornerNWSE: return this->m_lookupCornersDiag[OrientabilityIndexDiagon::DiagNW];
			case Facings::CornerSWNE: return this->m_lookupCornersDiag[OrientabilityIndexDiagon::DiagSW];
			case Facings::Corner3NW: return this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagNW];
			case Facings::Corner3NE: return this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagNE];
			case Facings::Corner3SW: return this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagSW];
			case Facings::Corner3SE: return this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagSE];
		}
	}

	return this->m_lookupShy;
}

Facings Dings::AvailableFacings()
{
	return this->m_Facings;
}

bool Dings::CouldCoalesce()
{
	return this->m_Coalescing != Facings::Shy && this->m_Coalescing != Facings::Viech;
}