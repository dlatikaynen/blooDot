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

void DialogOverlay::CalculateSize()
{
	ElementBase::CalculateSize();
	auto d2dContext = UserInterface::GetD2DContext();
	this->m_size = SizeF(
		this->m_sizeClientarea.width + 20.0F,
		this->m_sizeClientarea.height + 40.0F
	);

	this->CreateTextLayout();
}

void DialogOverlay::SetClientareaSize(D2D1_SIZE_F clientAreaSize)
{
	this->m_sizeClientarea = clientAreaSize;
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

	auto screenSize = d2dContext->GetSize();
	auto screenRect = D2D1::RectF(0.0F, 0.0F, screenSize.width - 1.0F, screenSize.height - 1.0F);
	d2dContext->FillRectangle(screenRect, this->m_coverBrush.Get());
	d2dContext->FillRectangle(bounds, this->m_chromeBrush.Get());
	d2dContext->DrawRectangle(bounds, this->m_blackBrush.Get());
	//d2dContext->DrawRectangle(D2D1::RectF(this->client.left - 1.0F, bounds.top - ), this->m_blackBrush.Get());
	this->RenderClientarea(d2dContext);
}

void DialogOverlay::RenderClientarea(ID2D1DeviceContext* d2dContext)
{

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