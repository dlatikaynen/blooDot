#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

const int Dings::MAUER_GRAY_SOLID = 128;
const int Dings::MAUER_GRAY_LOOSE = 98;
const float Dings::MAUER_INDENT = 5.f;
const float Dings::MAUER_BEZIER_X = 8.f;
const float Dings::MAUER_BEZIER_Y = 20.f;

Mauer::Mauer(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(Dings::DingIDs::Mauer, "Mauer", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Immersed;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
	this->m_Behaviors = ObjectBehaviors::Solid;
}

void Mauer::PrepareBackground(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	Microsoft::WRL::ComPtr<ID2D1Brush> brrect = this->m_Brushes->WannaHave(drawTo, { 
		Mauer::MAUER_GRAY_SOLID, 
		Mauer::MAUER_GRAY_SOLID, 
		Mauer::MAUER_GRAY_SOLID, 
		255 
	});

	auto rendEr = drawTo.Get();
	this->PrepareRect7x7(&this->m_lookupShy, rect);
	rendEr->FillRectangle(rect, brrect.Get());
}

void Mauer::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	MFARGB colrect;
	Microsoft::WRL::ComPtr<ID2D1Brush> brrect;
	auto rendEr = drawTo.Get();	

	/* SHY - inner solid;
	 * IMMERSED as well
	 * → implied by background */

	colrect = { 192, 192, 192, 192 };
	brrect = this->m_Brushes->WannaHave(drawTo, colrect);
	auto innerBrush = brrect.Get();

	/* CENTER CROSSING */
	rect.left = 49.0f * this->m_lookupCrossing.x;
	rect.top = 49.0f * this->m_lookupCrossing.y;
	rect.right = rect.left + 49.0f;
	rect.bottom = rect.top + 49.0f;	
	rendEr->DrawLine(D2D1::Point2F(rect.left, rect.top + Dings::MAUER_INDENT), D2D1::Point2F(rect.left + Dings::MAUER_INDENT, rect.top+ Dings::MAUER_INDENT), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.left+ Dings::MAUER_INDENT, rect.top + Dings::MAUER_INDENT), D2D1::Point2F(rect.left + Dings::MAUER_INDENT, rect.top), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + Dings::MAUER_INDENT), D2D1::Point2F(rect.right - Dings::MAUER_INDENT, rect.top + Dings::MAUER_INDENT), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.right - Dings::MAUER_INDENT, rect.top + Dings::MAUER_INDENT), D2D1::Point2F(rect.right - Dings::MAUER_INDENT, rect.top), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.left, rect.bottom - Dings::MAUER_INDENT), D2D1::Point2F(rect.left + Dings::MAUER_INDENT, rect.bottom - Dings::MAUER_INDENT), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.left + Dings::MAUER_INDENT, rect.bottom - Dings::MAUER_INDENT), D2D1::Point2F(rect.left + Dings::MAUER_INDENT, rect.bottom), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.right, rect.bottom - Dings::MAUER_INDENT), D2D1::Point2F(rect.right - Dings::MAUER_INDENT, rect.bottom - Dings::MAUER_INDENT), innerBrush, 2.5f, 0);
	rendEr->DrawLine(D2D1::Point2F(rect.right - Dings::MAUER_INDENT, rect.bottom - Dings::MAUER_INDENT), D2D1::Point2F(rect.right - Dings::MAUER_INDENT, rect.bottom), innerBrush, 2.5f, 0);
	
	/* SHY
	 * inner border */
	rect.left = 49.0f * this->m_lookupShy.x + Dings::MAUER_INDENT;
	rect.top = 49.0f * this->m_lookupShy.y + Dings::MAUER_INDENT;
	rect.right = rect.left + 49.0f - 10.0f;
	rect.bottom = rect.top + 49.0f - 10.0f;
	rendEr->DrawRectangle(rect, innerBrush, 2.5f, 0);

	/* Us: rotate so we have lefty (no rotation) last, so
	 * after the loop ends the context is rotation-free */
	for (int facing = OrientabilityIndexQuadruplet::Uppy; facing >= OrientabilityIndexQuadruplet::Lefty; --facing)
	{
		this->PrepareRect(&this->m_lookupU[facing], rect);
		this->Rotate90(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5.0f, rect.top + 5.0f), D2D1::Point2F(rect.right, rect.top + 5.0f), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5.0f, rect.top + 5.0f), D2D1::Point2F(rect.left + 5, rect.bottom - 5.0f), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5.0f, rect.bottom - 5.0f), D2D1::Point2F(rect.right, rect.bottom - 5.0f), innerBrush, 2.5f, 0);
		/* Ts */
		this->PrepareRect(&this->m_lookupTs[facing], rect);
		this->Rotate90(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom), innerBrush, 2.5f, 0);
		/* single outer edges (sides) */
		this->PrepareRect(&this->m_lookupSides[facing], rect);
		this->Rotate90(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
		/* double corners */
		this->PrepareRect(&this->m_lookupCornersInner2[facing], rect);
		this->Rotate90(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom), innerBrush, 2.5f, 0);
	}

	/* pipes and double inner corners (dually oriented specialities) */
	for (int facing = OrientabilityIndexDuplex::Vertically; facing >= OrientabilityIndexDuplex::Horizontally; --facing)
	{
		this->PrepareRect(&this->m_lookupPipes[facing], rect);
		this->Rotate90(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left, rect.top + 5), D2D1::Point2F(rect.right, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left, rect.bottom - 5), D2D1::Point2F(rect.right, rect.bottom - 5), innerBrush, 2.5f, 0);
		this->PrepareRect(&this->m_lookupCornersDiag[facing], rect);
		this->Rotate90(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left, rect.bottom - 5), D2D1::Point2F(rect.left + 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.bottom - 5), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
	}

	for (int facing = OrientabilityIndexDiagon::DiagNE; facing >= OrientabilityIndexDiagon::DiagNW; --facing)
	{
		/* outer-only edges (diagonal) */
		this->PrepareRect(&this->m_lookupEdgesOuter90[facing], rect);
		this->Rotate90(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.right, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
		/* genuine edges (knees) */
		this->PrepareRect(&this->m_lookupEdges[facing], rect);
		this->Rotate90(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.right, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom), innerBrush, 2.5f, 0);
		/* single edges with one inner corner, near */
		this->PrepareRect(&this->m_lookupEdgesInner1[facing], rect);
		this->Rotate90(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top), innerBrush, 2.5f, 0);
		/* single edges with one inner corner, far */
		this->PrepareRect(&this->m_lookupEdgesInner1[facing + FAR_OFFSET], rect);
		this->Rotate90(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom), innerBrush, 2.5f, 0);
		/* double edges with opposing single corner */
		this->PrepareRect(&this->m_lookupCornersBoth90[facing], rect);
		this->Rotate90(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.right, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.bottom - 5), D2D1::Point2F(rect.right - 5, rect.bottom), innerBrush, 2.5f, 0);
		/* single corners */
		this->PrepareRect(&this->m_lookupCornersInner1[facing], rect);
		this->Rotate90(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top), innerBrush, 2.5f, 0);
		/* triple corners */
		this->PrepareRect(&this->m_lookupCornersInner3[facing], rect);
		this->Rotate90(rendEr, rect, facing);
		rendEr->DrawLine(D2D1::Point2F(rect.right, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.right - 5, rect.top + 5), D2D1::Point2F(rect.right - 5, rect.top), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.top + 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.top + 5), D2D1::Point2F(rect.left + 5, rect.top), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left, rect.bottom - 5), D2D1::Point2F(rect.left + 5, rect.bottom - 5), innerBrush, 2.5f, 0);
		rendEr->DrawLine(D2D1::Point2F(rect.left + 5, rect.bottom - 5), D2D1::Point2F(rect.left + 5, rect.bottom), innerBrush, 2.5f, 0);
	}
}

CrackedMauer::CrackedMauer(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Mauer(deviceResources, drawBrushes)
{
	this->m_ID = Dings::DingIDs::MauerCracked;
	this->m_Name = L"Cracked";
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Immersed;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::Shootable;
}

void CrackedMauer::PrepareBackground(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	auto backPic = this->LoadBitmap(L"packcover-wallcrack.png");
	auto rendEr = drawTo.Get();

	Mauer::PrepareBackground(drawTo);
	this->PrepareRect7x7(&this->m_lookupShy, rect);
	rendEr->DrawBitmap(backPic.Get(), rect);
}

LooseMauer::LooseMauer(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(Dings::DingIDs::MauerLoose, "Loose", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::Pushable;
}

void LooseMauer::PrepareBackground(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	Microsoft::WRL::ComPtr<ID2D1Brush> brrect = this->m_Brushes->WannaHave(drawTo, 
	{
		Mauer::MAUER_GRAY_LOOSE,
		Mauer::MAUER_GRAY_LOOSE,
		Mauer::MAUER_GRAY_LOOSE,
		255
	});

	auto rendEr = drawTo.Get();
	this->PrepareRect(&this->m_lookupShy, rect);
	rect.left++;
	rect.top++;
	rect.right--;
	rect.bottom--;
	rendEr->FillRoundedRectangle(D2D1::RoundedRect(rect, 3.0f, 3.0f), brrect.Get());
}

void LooseMauer::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	Microsoft::WRL::ComPtr<ID2D1PathGeometry> arcSegments;
	Microsoft::WRL::ComPtr<ID2D1GeometrySink> geometrySink;
	Microsoft::WRL::ComPtr<ID2D1Brush> brrect;
	auto rendEr = drawTo.Get();
	DX::ThrowIfFailed
	(
		this->m_deviceResources->GetD2DFactory()->CreatePathGeometry(arcSegments.GetAddressOf())
	);

	DX::ThrowIfFailed
	(
		arcSegments->Open(geometrySink.GetAddressOf())
	);
	
	/* we indent the sides to suggest cushion */
	this->PrepareRect(&this->m_lookupShy, rect);
	geometrySink->BeginFigure(
		D2D1::Point2F(rect.left + Mauer::MAUER_INDENT, rect.top + Mauer::MAUER_INDENT),
		D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_HOLLOW
	);

	geometrySink->AddBezier(D2D1::BezierSegment(
		D2D1::Point2F(rect.left + Mauer::MAUER_BEZIER_Y, rect.top + Mauer::MAUER_BEZIER_X),
		D2D1::Point2F(rect.right - Mauer::MAUER_BEZIER_Y, rect.top + Mauer::MAUER_BEZIER_X),
		D2D1::Point2F(rect.right - Mauer::MAUER_INDENT, rect.top + Mauer::MAUER_INDENT)
	));

	geometrySink->AddBezier(D2D1::BezierSegment(
		D2D1::Point2F(rect.right - Mauer::MAUER_BEZIER_X, rect.top + Mauer::MAUER_BEZIER_Y),
		D2D1::Point2F(rect.right - Mauer::MAUER_BEZIER_X, rect.bottom - Mauer::MAUER_BEZIER_Y),
		D2D1::Point2F(rect.right - Mauer::MAUER_INDENT, rect.bottom - Mauer::MAUER_INDENT)
	));

	geometrySink->AddBezier(D2D1::BezierSegment(
		D2D1::Point2F(rect.left + Mauer::MAUER_BEZIER_Y, rect.bottom - Mauer::MAUER_BEZIER_X),
		D2D1::Point2F(rect.right - Mauer::MAUER_BEZIER_Y, rect.bottom - Mauer::MAUER_BEZIER_X),
		D2D1::Point2F(rect.left + Mauer::MAUER_INDENT, rect.bottom - Mauer::MAUER_INDENT)
	));

	geometrySink->AddBezier(D2D1::BezierSegment(
		D2D1::Point2F(rect.left + Mauer::MAUER_BEZIER_X, rect.top + Mauer::MAUER_BEZIER_Y),
		D2D1::Point2F(rect.left + Mauer::MAUER_BEZIER_X, rect.bottom - Mauer::MAUER_BEZIER_Y),
		D2D1::Point2F(rect.left + Mauer::MAUER_INDENT, rect.top + Mauer::MAUER_INDENT)
	));

	geometrySink->EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED);
	DX::ThrowIfFailed
	(
		geometrySink->Close()
	);

	auto colrect = MFARGB { 192, 192, 192, 192 };
	brrect = this->m_Brushes->WannaHave(drawTo, colrect);
	auto innerBrush = brrect.Get();
	rendEr->DrawGeometry(arcSegments.Get(), innerBrush, 2.5f);
}