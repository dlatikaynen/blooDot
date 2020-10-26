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

void DialogOverlay::Initialize(std::shared_ptr<BrushRegistry> brushRegistry)
{
	ElementBase::Initialize(brushRegistry);
	auto d2dContext = UserInterface::GetD2DContext();
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(RGB(10,10,10)), &this->m_textColorBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::Black), &this->m_shadowColorBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &this->m_blackBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gold), &this->m_chromeBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &this->m_coverBrush));
	this->m_shadowColorBrush->SetOpacity(m_shadowColorBrush->GetOpacity() * blooDot::Consts::GOLDEN_RATIO);
	this->m_coverBrush->SetOpacity(m_coverBrush->GetOpacity() * blooDot::Consts::INVERSE_GOLDEN_RATIO);
	this->SetCaption(this->StaticCaption());
	this->SetVisible(false);
}

Platform::String^ DialogOverlay::StaticCaption()
{
	return L"eew, pure virtual dialog caption";
}

void DialogOverlay::SetCaption(Platform::String^ value)
{ 
	this->m_Caption = value; 
	if (this->m_textLayout != nullptr)
	{
		this->m_textLayout.Reset();
		this->m_textLayout = nullptr;
		this->CreateTextLayout();
	}
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

	this->m_outerBounds = this->GetBounds();
	this->m_clientArea.left = this->m_outerBounds.left + CHROMEWIDTH;
	this->m_clientArea.top = this->m_outerBounds.top + CHROMEWIDTH + CAPTIONHEIGHT;
	this->m_clientArea.right = this->m_outerBounds.right - CHROMEWIDTH;
	this->m_clientArea.bottom = this->m_outerBounds.bottom - CHROMEWIDTH;
	this->CreateTextLayout();
}

void DialogOverlay::CalculateSize()
{
	ElementBase::CalculateSize();
}

void DialogOverlay::CreateTextLayout()
{	
	if (this->m_textLayout == nullptr)
	{
		auto dwriteFactory = UserInterface::GetDWriteFactory();
		this->m_textStyle.SetFontName(L"Fredoka One");
		this->m_textStyle.SetFontSize(12.0F);
		DX::ThrowIfFailed(
			dwriteFactory->CreateTextLayout(
				this->GetCaption()->Data(),
				this->GetCaption()->Length(),
				this->m_textStyle.GetTextFormat(),
				this->m_outerBounds.right - this->m_outerBounds.left,
				CAPTIONHEIGHT,
				&this->m_textLayout
			)
		);

		this->m_textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING);
		this->m_textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER);		
	}
}

void DialogOverlay::ScheduleDialogCommand(blooDot::DialogCommand dialogCommand)
{
	if (this->m_pendingDialogCommand == blooDot::DialogCommand::None)
	{
		this->m_pendingDialogCommand = dialogCommand;
	}
}

blooDot::DialogCommand DialogOverlay::DequeDialogCommand()
{
	auto pendingCommand = this->m_pendingDialogCommand;
	if (pendingCommand != blooDot::DialogCommand::None)
	{
		this->m_pendingDialogCommand = blooDot::DialogCommand::None;
	}

	return pendingCommand;
}

void DialogOverlay::Update(float timeTotal, float timeDelta)
{
	ElementBase::Update(timeTotal, timeDelta);


}

void DialogOverlay::Render()
{
	ElementBase::Render();
	auto d2dContext = UserInterface::GetD2DContext();
	auto screenSize = d2dContext->GetSize();
	auto screenRect = D2D1::RectF(0.0F, 0.0F, screenSize.width - 1.0F, screenSize.height - 1.0F);
	d2dContext->FillRectangle(screenRect, this->m_coverBrush.Get());
	d2dContext->FillRectangle(this->m_outerBounds, this->m_chromeBrush.Get());
	d2dContext->DrawRectangle(this->m_outerBounds, this->m_blackBrush.Get());
	d2dContext->DrawRectangle(D2D1::RectF(this->m_clientArea.left - 1.0F, this->m_clientArea.top - 1.0F, this->m_clientArea.right + 1.0F, this->m_clientArea.bottom + 1.0F), this->m_blackBrush.Get());
	this->RenderClientarea(d2dContext);
	d2dContext->DrawTextLayout(
		Point2F(
			this->m_outerBounds.left + CHROMEWIDTH - 2.0F,
			this->m_outerBounds.top + CHROMEWIDTH / 2.0F
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