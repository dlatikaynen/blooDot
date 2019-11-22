#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"
#include "LevelEditorHUD.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

LevelEditorHUD::LevelEditorHUD()
{
	this->m_isEraserChosen = false;
	this->m_selectedDingID = 0;
}

void LevelEditorHUD::Initialize()
{
	auto d2dContext = UserInterface::GetD2DContext();
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::White), &this->m_textColorBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::Black), &this->m_shadowColorBrush));
	this->m_shadowColorBrush->SetOpacity(m_textColorBrush->GetOpacity() * blooDot::Consts::GOLDEN_RATIO);
}

void LevelEditorHUD::CalculateSize()
{
	auto d2dContext = UserInterface::GetD2DContext();
	this->m_size = SizeF(
		d2dContext->GetSize().width / 2.0F,
		blooDot::Consts::SQUARE_HEIGHT * 2.0F
	);
}

bool LevelEditorHUD::IsDingSelected()
{
	return this->m_selectedDingID > 0;
}

void LevelEditorHUD::SelectDing(unsigned dingID, Microsoft::WRL::ComPtr<ID2D1Bitmap> dingImage)
{
	this->m_selectedDingID = dingID;
	this->m_selectedDingImage = dingImage;
}

void LevelEditorHUD::Update(float timeTotal, float timeDelta)
{
	

}

void LevelEditorHUD::Render()
{
	auto d2dContext = UserInterface::GetD2DContext();
	D2D1_RECT_F bounds = this->GetBounds();
	d2dContext->FillRectangle(&bounds, this->m_shadowColorBrush.Get());

	if (this->m_selectedDingID > 0)
	{
		auto dingPic = this->m_selectedDingImage.Get();
		d2dContext->DrawBitmap(dingPic, D2D1::RectF(bounds.left, bounds.top, bounds.left + blooDot::Consts::SQUARE_WIDTH, bounds.top + blooDot::Consts::SQUARE_HEIGHT));
	}
}

void LevelEditorHUD::ReleaseDeviceDependentResources()
{
	this->m_textColorBrush.Reset();
	this->m_shadowColorBrush.Reset();
	this->m_selectedDingImage.Reset();
}

unsigned LevelEditorHUD::SelectedDingID()
{
	return this->m_selectedDingID;
}

void LevelEditorHUD::ToggleEraser(bool doErase)
{
	this->m_isEraserChosen = doErase;
}

bool LevelEditorHUD::IsInEraserMode()
{
	return this->m_isEraserChosen;
}