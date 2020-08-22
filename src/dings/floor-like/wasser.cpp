#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

Wasser::Wasser(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(Dings::DingIDs::Wasser, "Wasser", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = Layers::Floor;
	this->m_Behaviors =
		ObjectBehaviors::Immersible |
		ObjectBehaviors::CreatesDrag;
}

void Wasser::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	MFARGB colrect { 192, 0, 0, 255 };
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl;
	PrepareRect(&this->m_lookupShy, rect);
	brusherl = m_Brushes->WannaHave(drawTo, colrect);
	auto innerBrush = brusherl.Get();
	auto rendEr = drawTo.Get();
	rendEr->FillRectangle(rect, innerBrush);
}

Rail::Rail(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(Dings::DingIDs::Rail, "Rail", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Center;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = Layers::Floor;
	this->m_Behaviors = ObjectBehaviors::Solid;
}

// the horizontal one is needed, vertical one is created by the framework through rotation
void Rail::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, const D2D1_RECT_F *rect)
{
	// a nice, astatine-colored rail
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl = m_Brushes->WannaHave(drawTo, MFARGB { 204, 204, 192, 255 });
	auto railBrush = brusherl.Get();
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl2 = m_Brushes->WannaHave(drawTo, MFARGB { 34, 48, 68, 255 });
	auto sleeperBrush = brusherl2.Get();
	auto rendEr = drawTo.Get();
	rendEr->FillRectangle(D2D1::RectF(rect->left + 10.0f, rect->top + 11.5f, rect->left + 14.0f, rect->bottom - 11.5f), sleeperBrush);
	rendEr->FillRectangle(D2D1::RectF(rect->left + 23.0f, rect->top + 11.5f, rect->left + 27.0f, rect->bottom - 11.5f), sleeperBrush);
	rendEr->FillRectangle(D2D1::RectF(rect->right - 14.0f, rect->top + 11.5f, rect->right - 10.0f, rect->bottom - 11.5f), sleeperBrush);
	rendEr->FillRectangle(D2D1::RectF(rect->left, rect->top + 13.0f, rect->right, rect->top + 14.0f), railBrush);
	rendEr->FillRectangle(D2D1::RectF(rect->left, rect->bottom - 14.0f, rect->right, rect->bottom - 13.0f), railBrush);
}