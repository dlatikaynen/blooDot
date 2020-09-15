#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

Door::Door(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Dings(Dings::DingIDs::Door, "Door", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Center;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
	this->m_Behaviors = 
		ObjectBehaviors::Solid 
		| ObjectBehaviors::Openable;
}

void Door::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, const D2D1_RECT_F *rect)
{
	auto rendEr = drawTo.Get();
	auto colGrid = MFARGB{ 192, 192, 192, 192 };
	auto brGrid = this->m_Brushes->WannaHave(drawTo, colGrid);
	auto innerBrush = brGrid.Get();
	rendEr->DrawLine(
		D2D1::Point2F(rect->left + 20.f, rect->top),
		D2D1::Point2F(rect->left + 20.f, rect->bottom),
		innerBrush, 
		2.5f
	);

	rendEr->DrawLine(
		D2D1::Point2F(rect->left + 24.5f, rect->top),
		D2D1::Point2F(rect->left + 24.5f, rect->bottom),
		innerBrush,
		2.5f
	);

	rendEr->DrawLine(
		D2D1::Point2F(rect->right - 20.f, rect->top),
		D2D1::Point2F(rect->right - 20.f, rect->bottom),
		innerBrush,
		2.5f
	);
}