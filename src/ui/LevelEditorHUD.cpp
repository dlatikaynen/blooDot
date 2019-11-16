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

void LevelEditorHUD::Update(float timeTotal, float timeDelta)
{
	

}

void LevelEditorHUD::Render()
{
	auto d2dContext = UserInterface::GetD2DContext();
	D2D1_RECT_F bounds = this->GetBounds();
	d2dContext->FillRectangle(&bounds, this->m_shadowColorBrush.Get());
}

void LevelEditorHUD::ReleaseDeviceDependentResources()
{
	m_textColorBrush.Reset();
	m_shadowColorBrush.Reset();
}

void LevelEditorHUD::ToggleEraser()
{
	this->m_isEraserChosen = !this->m_isEraserChosen;
}

bool LevelEditorHUD::IsInEraserMode()
{
	return this->m_isEraserChosen;
}