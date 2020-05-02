#include "..\PreCompiledHeaders.h"
#include "Dings.h"
#include <math.h>

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

Dings::Dings(Dings::DingIDs dingID, Platform::String^ dingName, std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes)
{
	this->m_ID = dingID;
	this->m_Name = dingName;
	this->m_deviceResources = deviceResources;
	this->m_Brushes = drawBrushes;
	this->m_Behaviors = ObjectBehaviors::Boring;
}

Dings::DingIDs Dings::ID()
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

ObjectBehaviors Dings::GetInherentBehaviors()
{
	return this->m_Behaviors;
}

void Dings::Draw(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, int canvasX, int canvasY)
{
	this->m_lookupShy.x = canvasX;
	this->m_lookupShy.y = canvasY;
	this->SetSheetPlacementsFromCoalescability();
	this->m_fromFile = this->ShouldLoadFromBitmap();
	this->PrepareBackground(drawTo);
	if (this->m_Facings == Facings::Cross)
	{
		this->DrawQuadruplet(drawTo);
	}
	else if (this->m_Facings == Facings::Viech)
	{
		this->DrawRotatory(drawTo);
	}
	else if (this->m_Facings == Facings::Center)
	{
		this->DrawTwin(drawTo);
	}
	else if (this->m_Facings != Facings::Shy)
	{
		this->DrawClumsyPack(drawTo);
	}
	else
	{
		this->DrawInternal(drawTo);
	}
}

Platform::String^ Dings::ShouldLoadFromBitmap()
{
	return nullptr;
}

Microsoft::WRL::ComPtr<ID2D1Bitmap> Dings::LoadBitmap(Platform::String^ fileName)
{
	Microsoft::WRL::ComPtr<ID2D1Bitmap>	bitMap;
	auto loader = ref new BasicLoader(this->m_deviceResources->GetD3DDevice());
	auto fullPath = Platform::String::Concat(L"Media\\Bitmaps\\", fileName);
	loader->LoadPngToBitmap(fullPath, this->m_deviceResources, &bitMap);
	return bitMap;
}

Microsoft::WRL::ComPtr<ID2D1Bitmap> Dings::LoadFromBitmap()
{	
	return this->LoadBitmap(this->m_fromFile);
}

void Dings::DrawShy(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	PrepareRect(&this->m_lookupShy, rect);
	this->DrawInternal(drawTo, rect);
}

void Dings::DrawTwin(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	Microsoft::WRL::ComPtr<ID2D1Bitmap> bitMap;
	auto rendEr = drawTo.Get();
	if (this->m_fromFile != nullptr)
	{
		bitMap = this->LoadFromBitmap();
	}

	for (int orientationDent = OrientabilityIndexDuplex::Vertically; orientationDent >= OrientabilityIndexDuplex::Horizontally; --orientationDent)
	{
		this->PrepareRect(&this->m_lookupPipes[orientationDent], rect);
		this->Rotate90(rendEr, rect, orientationDent);
		if (this->m_fromFile == nullptr)
		{
			this->DrawInternal(drawTo, rect);
		}
		else
		{
			drawTo->DrawBitmap(bitMap.Get(), rect);
		}
	}
	
	bitMap.Reset();
}

void Dings::DrawQuadruplet(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	Microsoft::WRL::ComPtr<ID2D1Bitmap> bitMap;
	auto rendEr = drawTo.Get();
	if (this->m_fromFile != nullptr)
	{
		bitMap = this->LoadFromBitmap();
	}

	for (int orientationDent = OrientabilityIndexQuadruplet::Uppy; orientationDent >= OrientabilityIndexQuadruplet::Lefty; --orientationDent)
	{
		this->PrepareRect(&this->m_lookupSides[orientationDent], rect);
		this->Rotate90(rendEr, rect, orientationDent);
		if (this->m_fromFile == nullptr)
		{
			this->DrawInternal(drawTo, rect);
		}
		else
		{
			drawTo->DrawBitmap(bitMap.Get(), rect);
		}
	}
	
	bitMap.Reset();
}

void Dings::DrawRotatory(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	Microsoft::WRL::ComPtr<ID2D1Bitmap> bitMap;
	auto rendEr = drawTo.Get();
	if (this->m_fromFile != nullptr)
	{
		bitMap = this->LoadFromBitmap();
	}

	for (int orientationDent = OrientabilityIndexRotatory::HDG360; orientationDent <= OrientabilityIndexRotatory::HDG337_5; ++orientationDent)
	{
		/* all base mobs are lefty, while our starting index is a HDG 360,
		 * so we add 90° to initial rotation to match it with reality */
		this->PrepareRect(&this->m_lookupSides[orientationDent], rect);
		this->Rotate(rendEr, rect, static_cast<float>(orientationDent) * 22.5F + 90.0F);
		if (this->m_fromFile == nullptr)
		{
			this->DrawInternal(drawTo, rect);
		}
		else
		{
			drawTo->DrawBitmap(bitMap.Get(), rect);
		}
	}

	bitMap.Reset();
}

void Dings::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	Microsoft::WRL::ComPtr<ID2D1Bitmap> bitMap;
	auto rendEr = drawTo.Get();
	if (this->m_fromFile != nullptr)
	{
		bitMap = this->LoadFromBitmap();
	}

	PrepareRect(&this->m_lookupShy, rect);
	if (this->m_fromFile == nullptr)
	{
		this->DrawInternal(drawTo, rect);
	}
	else
	{
		drawTo->DrawBitmap(bitMap.Get(), rect);
	}

	bitMap.Reset();
}

void Dings::DrawClumsyPack(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	/* this one must deal with all its 7x7 complexity all by itself */	
	this->DrawInternal(drawTo);
}

void Dings::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, D2D1_RECT_F rect)
{
	/* pure virtual */
}

void Dings::SetSheetPlacementsFromCoalescability()
{
	auto oX = this->m_lookupShy.x, oY = this->m_lookupShy.y;
	auto x = oX, y = oY;
	if (this->m_Facings == Facings::Cross)
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
	else if (this->m_Facings == Facings::Viech)
	{
		this->m_lookupSides[OrientabilityIndexRotatory::HDG360].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG360].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG22_5].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG22_5].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG45].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG45].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG67_5].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG67_5].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG90].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG90].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG112_5].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG112_5].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG135].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG135].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG157_5].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG157_5].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG180].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG180].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG202_5].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG202_5].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG225].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG225].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG247_5].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG247_5].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG270].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG270].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG292_5].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG292_5].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG315].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG315].y = y;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG337_5].x = x++;
		this->m_lookupSides[OrientabilityIndexRotatory::HDG337_5].y = y;
	}
	else if (this->m_Facings == Facings::Center)
	{
		this->m_lookupPipes[OrientabilityIndexDuplex::Horizontally].x = x++;
		this->m_lookupPipes[OrientabilityIndexDuplex::Horizontally].y = y;
		this->m_lookupPipes[OrientabilityIndexDuplex::Vertically].x = x;
		this->m_lookupPipes[OrientabilityIndexDuplex::Vertically].y = y;
	}
	else if (this->m_Coalescing != Facings::Shy) 
	{	
		this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupImmersed.x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupImmersed.y = y;
		this->m_lookupCrossing.x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCrossing.y = y;
		this->m_lookupPipes[OrientabilityIndexDuplex::Vertically].y = y;
		this->m_lookupPipes[OrientabilityIndexDuplex::Vertically].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupPipes[OrientabilityIndexDuplex::Horizontally].y = y;
		this->m_lookupPipes[OrientabilityIndexDuplex::Horizontally].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupU[OrientabilityIndexQuadruplet::Lefty].y = y;
		this->m_lookupU[OrientabilityIndexQuadruplet::Lefty].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupU[OrientabilityIndexQuadruplet::Righty].y = y;
		this->m_lookupU[OrientabilityIndexQuadruplet::Righty].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupU[OrientabilityIndexQuadruplet::Uppy].y = y;
		this->m_lookupU[OrientabilityIndexQuadruplet::Uppy].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupU[OrientabilityIndexQuadruplet::Downy].y = y;
		this->m_lookupU[OrientabilityIndexQuadruplet::Downy].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagNW].y = y;
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagNW].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagSW].y = y;
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagSW].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagNE].y = y;
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagNE].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagSE].y = y;
		this->m_lookupEdges[OrientabilityIndexDiagon::DiagSE].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupSides[OrientabilityIndexQuadruplet::Lefty].y = y;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Lefty].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupSides[OrientabilityIndexQuadruplet::Uppy].y = y;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Uppy].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupSides[OrientabilityIndexQuadruplet::Righty].y = y;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Righty].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupSides[OrientabilityIndexQuadruplet::Downy].y = y;
		this->m_lookupSides[OrientabilityIndexQuadruplet::Downy].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagNW].y = y;
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagNW].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagSW].y = y;
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagSW].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagNE].y = y;
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagNE].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagSE].y = y;
		this->m_lookupEdgesOuter90[OrientabilityIndexDiagon::DiagSE].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Lefty].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Lefty].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::LeftyFar].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::LeftyFar].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Righty].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Righty].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::RightyFar].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::RightyFar].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Downy].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Downy].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::DownyFar].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::DownyFar].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Uppy].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::Uppy].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::UppyFar].y = y;
		this->m_lookupEdgesInner1[OrientabilityIndexQuadruplet::UppyFar].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagNW].y = y;
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagNW].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagSE].y = y;
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagSE].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagNE].y = y;
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagNE].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagSW].y = y;
		this->m_lookupCornersBoth90[OrientabilityIndexDiagon::DiagSW].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupTs[OrientabilityIndexQuadruplet::Lefty].y = y;
		this->m_lookupTs[OrientabilityIndexQuadruplet::Lefty].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupTs[OrientabilityIndexQuadruplet::Righty].y = y;
		this->m_lookupTs[OrientabilityIndexQuadruplet::Righty].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupTs[OrientabilityIndexQuadruplet::Uppy].y = y;
		this->m_lookupTs[OrientabilityIndexQuadruplet::Uppy].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupTs[OrientabilityIndexQuadruplet::Downy].y = y;
		this->m_lookupTs[OrientabilityIndexQuadruplet::Downy].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagNW].y = y;
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagNW].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagNE].y = y;
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagNE].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagSW].y = y;
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagSW].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagSE].y = y;
		this->m_lookupCornersInner1[OrientabilityIndexDiagon::DiagSE].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Lefty].y = y;
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Lefty].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Righty].y = y;
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Righty].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Downy].y = y;
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Downy].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Uppy].y = y;
		this->m_lookupCornersInner2[OrientabilityIndexQuadruplet::Uppy].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersDiag[OrientabilityIndexDiagon::DiagNW].y = y;
		this->m_lookupCornersDiag[OrientabilityIndexDiagon::DiagNW].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersDiag[OrientabilityIndexDiagon::DiagSW].y = y;
		this->m_lookupCornersDiag[OrientabilityIndexDiagon::DiagSW].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagNW].y = y;
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagNW].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagNE].y = y;
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagNE].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagSW].y = y;
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagSW].x = this->Pack7x7X(oX, oY, &x, &y);
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagSE].y = y;
		this->m_lookupCornersInner3[OrientabilityIndexDiagon::DiagSE].x = this->Pack7x7X(oX, oY, &x, &y);
	}
}

unsigned Dings::Pack7x7X(unsigned offsetX, unsigned offsetY, unsigned* x, unsigned* y)
{
	auto xCoord = *x;
	this->Pack7x7(offsetX, offsetY, x, y);
	return xCoord;
}

unsigned Dings::Pack7x7Y(unsigned offsetX, unsigned offsetY, unsigned* x, unsigned* y)
{
	auto yCoord = *y;
	this->Pack7x7(offsetX, offsetY, x, y);
	return yCoord;
}

void Dings::Pack7x7(unsigned offsetX, unsigned offsetY, unsigned* x, unsigned* y)
{
	(*x)++;
	if ((*x) == offsetX + 7)
	{
		(*x) = offsetX;
		(*y)++;
	}	
}

void Dings::PrepareRect(D2D1_POINT_2U *lookupLocation, D2D1_RECT_F &rectToSet)
{
	rectToSet.left = 49.0f * lookupLocation->x;
	rectToSet.top = 49.0f * lookupLocation->y;
	rectToSet.right = rectToSet.left + 49.0f;
	rectToSet.bottom = rectToSet.top + 49.0f;
}

void Dings::PrepareRect7x7(D2D1_POINT_2U *lookupLocation, D2D1_RECT_F &rectToSet)
{
	rectToSet.left = 49.0f * lookupLocation->x;
	rectToSet.top = 49.0f * lookupLocation->y;
	rectToSet.right = rectToSet.left + 7.0f * 49.0f;
	rectToSet.bottom = rectToSet.top + 7.0f * 49.0f;
}

bool Dings::BoundingIsDefaultRect()
{
	return this->m_Bounding == nullptr;
}

D2D1_RECT_F Dings::GetBoundingOuterRim()
{
	if (this->m_Bounding == nullptr)
	{
		return D2D1::RectF(0.0F, 0.0F, blooDot::Consts::SQUARE_WIDTH, blooDot::Consts::SQUARE_HEIGHT);
	}
	else
	{
		return this->m_Bounding->OuterRim;
	}
}

std::shared_ptr<Bounding> Dings::GetBoundingInfo()
{
	if (this->m_Bounding == nullptr)
	{
		/* actually construct a default boundary object and deliver that */
		this->m_Bounding = std::make_shared<Bounding>();
		this->m_Bounding->OuterRim = D2D1::RectF(0.0F, 0.0F, blooDot::Consts::SQUARE_WIDTH, blooDot::Consts::SQUARE_HEIGHT);
	}

	return this->m_Bounding;
}

void Dings::Rotate90(ID2D1RenderTarget *rendEr, D2D1_RECT_F rect, int rotateTimes)
{
	if (rotateTimes == 0)
	{
		rendEr->SetTransform(D2D1::Matrix3x2F::Identity());
	}
	else
	{
		/* take anything else but 90° and funny stuff happens (heap corruption) */
		rendEr->SetTransform(D2D1::Matrix3x2F::Rotation(static_cast<float>(-90 * rotateTimes), D2D1::Point2F(rect.left + 24.5f, rect.top + 24.5f)));
	}
}

void Dings::Rotate(ID2D1RenderTarget *rendEr, D2D1_RECT_F rect, float rotationAngle)
{
	if (rotationAngle < 0.1F)
	{
		rendEr->SetTransform(D2D1::Matrix3x2F::Identity());
	}
	else
	{
		/* take anything else but 90° and funny stuff happens (heap corruption) */
		rendEr->SetTransform(D2D1::Matrix3x2F::Rotation(rotationAngle, D2D1::Point2F(rect.left + 24.5f, rect.top + 24.5f)));
	}
}

D2D1_POINT_2U Dings::GetSheetPlacement(OrientabilityIndexRotatory rotationDent)
{
	return this->m_lookupSides[rotationDent];
}

inline OrientabilityIndexRotatory Dings::HeadingFromFacing(Facings orientationFacing)
{
	switch (orientationFacing)
	{
		case Facings::North: return OrientabilityIndexRotatory::HDG360;
		case Facings::NNE: return OrientabilityIndexRotatory::HDG22_5;
		case Facings::NE: return OrientabilityIndexRotatory::HDG45;
		case Facings::NEE: return OrientabilityIndexRotatory::HDG67_5;
		case Facings::East: return OrientabilityIndexRotatory::HDG90;
		case Facings::SEE: return OrientabilityIndexRotatory::HDG112_5;
		case Facings::SE: return OrientabilityIndexRotatory::HDG135;
		case Facings::SSE: return OrientabilityIndexRotatory::HDG157_5;
		case Facings::South: return OrientabilityIndexRotatory::HDG180;
		case Facings::SSW: return OrientabilityIndexRotatory::HDG202_5;
		case Facings::SW: return OrientabilityIndexRotatory::HDG225;
		case Facings::SWW: return OrientabilityIndexRotatory::HDG247_5;
		case Facings::West: return OrientabilityIndexRotatory::HDG270;
		case Facings::NWW: return OrientabilityIndexRotatory::HDG292_5;
		case Facings::NW: return OrientabilityIndexRotatory::HDG315;
		case Facings::NNW: return OrientabilityIndexRotatory::HDG337_5;
#ifdef _DEBUG
		default:
			throw ref new Platform::FailureException(L"Invalid orientation in HeadingFromFacing function");
#endif
	}
}

D2D1_POINT_2U Dings::GetSheetPlacement(Facings orientation)
{
	if (this->m_Facings == Facings::Cross)
	{
		switch (orientation)
		{
			case Facings::West: return this->m_lookupSides[OrientabilityIndexQuadruplet::Lefty];
			case Facings::North:return this->m_lookupSides[OrientabilityIndexQuadruplet::Uppy];
			case Facings::East: return this->m_lookupSides[OrientabilityIndexQuadruplet::Righty];
			case Facings::South: return this->m_lookupSides[OrientabilityIndexQuadruplet::Downy];
		}
	}
	else if (this->m_Facings == Facings::Viech)
	{
		if (orientation == Facings::Shy)
		{
			orientation = Dings::DefaultMobFacing;
		}

		return this->m_lookupSides[Dings::HeadingFromFacing(orientation)];
	}
	else if ((this->m_Facings == Facings::Shy && this->m_Coalescing == Facings::Immersed) || this->m_Facings == Facings::Center)
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
	switch (this->m_Coalescing)
	{
		case Facings::Shy:
		case Facings::Cross:
		case Facings::Viech:
			return false;
	}

	return true;
}

Facings Dings::RotateFromFacing(Facings fromFacing, bool inverseDirection)
{
	/* pipes just flip, so they are always done with the inverse pattern as it makes zilch difference */
	if (inverseDirection || fromFacing == ::NS || fromFacing == ::WE)
	{
		switch (fromFacing)
		{
		case ::Shy:
		case ::West:
			return ::North;

		case ::South:
			return ::West;

		case ::East:
			return ::South;

		case ::North:
			return ::East;

		case ::NS:
			return ::WE;

		case ::WE:
			return ::NS;

		default:
			return fromFacing;
		}
	}
	else
	{
		switch (fromFacing)
		{
		case Facings::Shy:
		case Facings::West:
			return ::South;

		case Facings::South:
			return ::East;

		case Facings::East:
			return ::North;

		case Facings::North:
			return ::West;

		default:
			return fromFacing;
		}
	}
}
