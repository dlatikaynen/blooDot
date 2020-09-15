#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

Coin::Coin(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Dings(Dings::DingIDs::Coin, "Coin", deviceResources, drawBrushes)
{
	m_Facings = Facings::Shy;
	m_Coalescing = Facings::Shy;
	m_preferredLayer = Layers::Walls;
	m_possibleLayers = Layers::Walls;
	this->m_Behaviors = ObjectBehaviors::Takeable;

}

void Coin::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo)
{
	D2D1_RECT_F rect;
	MFARGB colrect { 10, 219, 221, 255 };
	MFARGB colring{ 12, 12, 12, 255 };
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl;
	PrepareRect(&this->m_lookupShy, rect);
	D2D1_ELLIPSE elli = D2D1::Ellipse(D2D1::Point2F(rect.left + (rect.right - rect.left) / 2.0f, rect.top + (rect.bottom - rect.top) / 2.0f), 10.0f, 10.0f);
	brusherl = m_Brushes->WannaHave(drawTo, colrect);
	auto innerBrush = brusherl.Get();
	auto rendEr = drawTo.Get();
	rendEr->FillEllipse(elli, innerBrush);
	brusherl = m_Brushes->WannaHave(drawTo, colring);
	auto ringBrush = brusherl.Get();
	elli.radiusX = elli.radiusY = (elli.radiusX - 2.5f);
	rendEr->DrawEllipse(elli, ringBrush, 0.9f);
	rendEr->DrawLine(D2D1::Point2F(elli.point.x + 0.5f, elli.point.y - 5.0f), D2D1::Point2F(elli.point.x + 0.5f, elli.point.y + 5.0f), ringBrush, 1.5f);
	rendEr->DrawLine(D2D1::Point2F(elli.point.x - 2.5f, elli.point.y - 3.0f), D2D1::Point2F(elli.point.x + 0.5f, elli.point.y - 5.0f), ringBrush, 1.5f);
}

Chest::Chest(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Dings(Dings::DingIDs::Chest, "Chest", deviceResources, drawBrushes)
{
	m_Facings = Facings::Cross;
	m_Coalescing = Facings::Shy;
	m_preferredLayer = Layers::Walls;
	m_possibleLayers = Layers::Walls;
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::Pushable |
		ObjectBehaviors::Openable |
		ObjectBehaviors::Closeable |
		ObjectBehaviors::CanPutStuffIn |
		ObjectBehaviors::CanTakeStuffOut;
}

int Chest::ColorVariation()
{
	return 1;
}

// the lefty representation (assuming placement against a wall)
void Chest::DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, const D2D1_RECT_F *rect)
{

	/* prepare */
	auto rendEr = drawTo.Get();
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl;
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl2;
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl3;
	Microsoft::WRL::ComPtr<ID2D1Brush> brusherl4;
	if (this->ColorVariation() == 3)
	{
		/* gold */
		brusherl = m_Brushes->WannaHave(drawTo, MFARGB{ 0, 225, 255, 255 });
		brusherl2 = m_Brushes->WannaHave(drawTo, MFARGB{ 128, 128, 128, 255 });
		brusherl3 = m_Brushes->WannaHave(drawTo, MFARGB{ 205, 205, 205, 255 });
		brusherl4 = m_Brushes->WannaHave(drawTo, MFARGB{ 5,5,84, 255 });
	}
	else if (this->ColorVariation() == 2)
	{
		/* silver */
		brusherl = m_Brushes->WannaHave(drawTo, MFARGB{ 140, 140, 140, 255 });
		brusherl2 = m_Brushes->WannaHave(drawTo, MFARGB{ 128, 128, 128, 255 });
		brusherl3 = m_Brushes->WannaHave(drawTo, MFARGB{ 205, 205, 205, 255 });
		brusherl4 = m_Brushes->WannaHave(drawTo, MFARGB{ 121, 132, 132, 255 });
	}
	else
	{
		/* wood */
		brusherl = m_Brushes->WannaHave(drawTo, MFARGB{ 0, 102, 153, 255 });
		brusherl2 = m_Brushes->WannaHave(drawTo, MFARGB{ 0, 91, 143, 255 });
		brusherl3 = m_Brushes->WannaHave(drawTo, MFARGB{ 51, 102, 102, 255 });
		brusherl4 = m_Brushes->WannaHave(drawTo, MFARGB{ 121, 132, 132, 255 });
	}

	
	auto brushBase = brusherl.Get();
	auto brushDark = brusherl2.Get();
	auto brushBark = brusherl3.Get();
	auto brushNail = brusherl4.Get();
	/* draw */
	auto left = rect->left + 1.0f;
	auto rite = rect->left + ((rect->right - rect->left) - 4.0f) * blooDot::Consts::GOLDEN_RATIO;
	auto yctr = rect->top + (rect->bottom - rect->top) / 2.0f;
	auto step = (rite - left) / 4.0f;
	rendEr->FillRectangle(D2D1::RectF(left, rect->top + 2.0f, rite, rect->bottom - 2.0f), brushBase);
	rendEr->DrawLine(D2D1::Point2F(rect->left + step, rect->top + 2.0f), D2D1::Point2F(rect->left + step, rect->bottom - 2.0f), brushDark);
	rendEr->DrawLine(D2D1::Point2F(rect->left + step * 2.0f, rect->top + 2.0f), D2D1::Point2F(rect->left + step * 2.0f, rect->bottom - 2.0f), brushDark);
	rendEr->DrawLine(D2D1::Point2F(rect->left + step * 3.0f, rect->top + 2.0f), D2D1::Point2F(rect->left + step * 3.0f, rect->bottom - 2.0f), brushDark);
	rendEr->FillRectangle(D2D1::RectF(left, rect->top + 3.0f, rite, rect->top + 10.0f), brushBark);
	rendEr->FillRectangle(D2D1::RectF(left, rect->bottom - 10.0f, rite, rect->bottom - 3.0f), brushBark);
	auto elli = D2D1::Ellipse(D2D1::Point2F(rect->left + step, rect->top + 6.5f), 1.3f, 1.3f);
	rendEr->FillEllipse(elli, brushNail);
	elli = D2D1::Ellipse(D2D1::Point2F(rect->left + step * 2.0f, rect->top + 6.5f), 1.3f, 1.3f);
	rendEr->FillEllipse(elli, brushNail);
	elli = D2D1::Ellipse(D2D1::Point2F(rect->left + step * 3.0f, rect->top + 6.5f), 1.3f, 1.3f);
	rendEr->FillEllipse(elli, brushNail);
	elli = D2D1::Ellipse(D2D1::Point2F(rect->left + step, rect->bottom - 6.5f), 1.3f, 1.3f);
	rendEr->FillEllipse(elli, brushNail);
	elli = D2D1::Ellipse(D2D1::Point2F(rect->left + step * 2.0f, rect->bottom - 6.5f), 1.3f, 1.3f);
	rendEr->FillEllipse(elli, brushNail);
	elli = D2D1::Ellipse(D2D1::Point2F(rect->left + step * 3.0f, rect->bottom - 6.5f), 1.3f, 1.3f);
	rendEr->FillEllipse(elli, brushNail);
	rendEr->FillRectangle(D2D1::RectF(rite, yctr - 1.8f, rite + 1.5f, yctr + 1.8f), brushNail);
}

SilverChest::SilverChest(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Chest(deviceResources, drawBrushes)
{
	this->m_ID = Dings::DingIDs::ChestSilver;
	this->m_Name = "Silver Chest";
	m_Facings = Facings::Cross;
	m_Coalescing = Facings::Shy;
	m_preferredLayer = Layers::Walls;
	m_possibleLayers = Layers::Walls;
}

int SilverChest::ColorVariation()
{
	return 2;
}

GoldChest::GoldChest(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Chest(deviceResources, drawBrushes)
{
	this->m_ID = Dings::DingIDs::ChestGold;
	this->m_Name = "Gold Chest";
	m_Facings = Facings::Cross;
	m_Coalescing = Facings::Shy;
	m_preferredLayer = Layers::Walls;
	m_possibleLayers = Layers::Walls;
}

int GoldChest::ColorVariation()
{
	return 3;
}

Lettuce::Lettuce(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Dings(Dings::DingIDs::Lettuce, "Lettuce", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::Takeable;
}

Platform::String^ Lettuce::ShouldLoadFromBitmap()
{
	return L"lettuce.png";
}

BarrelWooden::BarrelWooden(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Dings(Dings::DingIDs::BarrelWood, "Wooden barrel", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::Takeable;
}

Platform::String^ BarrelWooden::ShouldLoadFromBitmap()
{
	return L"barrel-wooden.png";
}

BarrelIndigo::BarrelIndigo(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes) : Dings(Dings::DingIDs::BarrelIndigo, "Indigo barrel", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Walls;
	this->m_possibleLayers = Layers::Walls;
	this->m_Behaviors =
		ObjectBehaviors::Solid |
		ObjectBehaviors::Takeable;
}

Platform::String^ BarrelIndigo::ShouldLoadFromBitmap()
{
	return L"barrel-indigo.png";
}
