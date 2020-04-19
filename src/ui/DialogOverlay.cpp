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
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::Black), &this->m_shadowColorBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &this->m_blackBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::PaleGoldenrod), &this->m_chromeBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &this->m_coverBrush));
	this->m_shadowColorBrush->SetOpacity(m_shadowColorBrush->GetOpacity() * blooDot::Consts::GOLDEN_RATIO);
	this->m_coverBrush->SetOpacity(m_coverBrush->GetOpacity() * blooDot::Consts::INVERSE_GOLDEN_RATIO);
	this->SetCaption(this->StaticCaption());
	this->SetVisible(false);
}

Platform::String^ DialogOverlay::StaticCaption()
{
	return L"";
}

void DialogOverlay::SetClientareaSize(D2D1_SIZE_F clientAreaSize)
{
	this->m_sizeClientarea = clientAreaSize;
	auto d2dContext = UserInterface::GetD2DContext();
	auto chromePadding = 2.0F * CHROMEWIDTH;
	this->m_size = SizeF(
		this->m_sizeClientarea.width + chromePadding,
		this->m_sizeClientarea.height + chromePadding + CAPTIONHEIGHT
	);

	auto windowBounds = this->GetBounds();
	this->m_clientArea.left = windowBounds.left + CHROMEWIDTH;
	this->m_clientArea.top = windowBounds.top + CHROMEWIDTH + CAPTIONHEIGHT;
	this->m_clientArea.right = windowBounds.right - CHROMEWIDTH;
	this->m_clientArea.bottom = windowBounds.bottom - CHROMEWIDTH;
}

void DialogOverlay::CalculateSize()
{
	ElementBase::CalculateSize();
	this->CreateTextLayout();
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
	auto screenSize = d2dContext->GetSize();
	auto screenRect = D2D1::RectF(0.0F, 0.0F, screenSize.width - 1.0F, screenSize.height - 1.0F);
	d2dContext->FillRectangle(screenRect, this->m_coverBrush.Get());
	d2dContext->FillRectangle(bounds, this->m_chromeBrush.Get());
	d2dContext->DrawRectangle(bounds, this->m_blackBrush.Get());
	d2dContext->DrawRectangle(D2D1::RectF(this->m_clientArea.left - 1.0F, this->m_clientArea.top - 1.0F, this->m_clientArea.right + 1.0F, this->m_clientArea.bottom + 1.0F), this->m_blackBrush.Get());
	this->RenderClientarea(d2dContext);
	d2dContext->DrawTextLayout(
		Point2F(
			bounds.left + CHROMEWIDTH,
			bounds.top + CHROMEWIDTH
		),
		this->m_textLayout.Get(),
		this->m_textColorBrush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
	);
}

void DialogOverlay::RenderClientarea(ID2D1DeviceContext* d2dContext)
{
	d2dContext->FillRectangle(this->m_clientArea, this->m_blackBrush.Get());
}

void DialogOverlay::ReleaseDeviceDependentResources()
{
	this->m_shadowColorBrush.Reset();
	this->m_textColorBrush.Reset();
	this->m_blackBrush.Reset();
	this->m_coverBrush.Reset();
	this->m_chromeBrush.Reset();
	this->m_textLayout.Reset();	
	ElementBase::ReleaseDeviceDependentResources();
}