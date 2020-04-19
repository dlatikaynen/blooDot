#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"
#include "DialogOverlay.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

DialogOverlay::DialogOverlay()
{
	this->m_visible = false;
}

void DialogOverlay::Initialize()
{
	ElementBase::Initialize();
	auto d2dContext = UserInterface::GetD2DContext();
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::White), &this->m_textColorBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::Black), &this->m_blackBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::Black), &this->m_shadowColorBrush));
	this->m_shadowColorBrush->SetOpacity(m_shadowColorBrush->GetOpacity() * blooDot::Consts::GOLDEN_RATIO);
	this->SetCaption(this->StaticCaption());
	this->SetVisible(false);
}

Platform::String^ DialogOverlay::StaticCaption()
{
	return L"";
}

void DialogOverlay::CalculateSize()
{
	ElementBase::CalculateSize();
	auto d2dContext = UserInterface::GetD2DContext();
	this->m_size = SizeF(
		d2dContext->GetSize().width / 2.0F,
		blooDot::Consts::SQUARE_HEIGHT * 2.0F
	);

	this->CreateTextLayout();
}

void DialogOverlay::SetClientareaSize(D2D1_RECT_F clientArea)
{
	this->m_sizeClientarea = clientArea;
}

void DialogOverlay::CreateTextLayout()
{	
	if (this->m_textLayout == nullptr)
	{
		auto dwriteFactory = UserInterface::GetDWriteFactory();
		this->m_textStyle.SetFontName(L"Fredoka One");
		DX::ThrowIfFailed(
			dwriteFactory->CreateTextLayout(
				this->GetCaption()->Data(),
				this->GetCaption()->Length(),
				this->m_textStyle.GetTextFormat(),
				this->m_container.right - this->m_container.left,
				this->m_container.bottom - this->m_container.top,
				&this->m_textLayout
			)
		);

		this->m_textColorBrush->SetColor(ColorF(ColorF::Aquamarine));
	}
}

void DialogOverlay::Update(float timeTotal, float timeDelta)
{
	ElementBase::Update(timeTotal, timeDelta);


}

void DialogOverlay::Render()
{
	ElementBase::Render();
	auto d2dContext = UserInterface::GetD2DContext();
	D2D1_RECT_F bounds = this->GetBounds();
	d2dContext->FillRectangle(&bounds, this->m_shadowColorBrush.Get());

	d2dContext->DrawTextLayout(
		Point2F(
			bounds.left + blooDot::Consts::DIALOG_PADDING,
			bounds.top + blooDot::Consts::DIALOG_PADDING / 2.0f + blooDot::Consts::SQUARE_HEIGHT + blooDot::Consts::DIALOG_PADDING
		),
		this->m_textLayout.Get(),
		this->m_textColorBrush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
	);

	this->RenderClientarea(d2dContext);
}

void DialogOverlay::RenderClientarea(ID2D1DeviceContext* d2dContext)
{

}

void DialogOverlay::ReleaseDeviceDependentResources()
{
	ElementBase::ReleaseDeviceDependentResources();
	this->m_shadowColorBrush.Reset();
	this->m_textColorBrush.Reset();
	this->m_blackBrush.Reset();
	this->m_coverBrush.Reset();
	this->m_chromeBrush.Reset();
}