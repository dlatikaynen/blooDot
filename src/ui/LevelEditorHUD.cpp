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
	this->m_selectedDingID = Dings::DingIDs::Void;
	this->m_isGridShown = true;
	this->m_isDingSheetShown = false;
	this->m_isSelectedDingRotatable = false;
}

void LevelEditorHUD::Initialize()
{
	auto d2dContext = UserInterface::GetD2DContext();
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::White), &this->m_textColorBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::Black), &this->m_shadowColorBrush));
	this->m_shadowColorBrush->SetOpacity(m_shadowColorBrush->GetOpacity() * blooDot::Consts::GOLDEN_RATIO);	
}

void LevelEditorHUD::CalculateSize()
{
	auto d2dContext = UserInterface::GetD2DContext();
	this->m_size = SizeF(
		d2dContext->GetSize().width / 2.0F,
		blooDot::Consts::SQUARE_HEIGHT * 2.0F
	);

	this->CreateTextLayout();
}

bool LevelEditorHUD::IsDingSelected()
{
	return this->m_selectedDingID > Dings::DingIDs::Void;
}

void LevelEditorHUD::SelectDing(Dings* ding, Microsoft::WRL::ComPtr<ID2D1Bitmap> dingImage, bool resetFacing)
{
	this->m_selectedDingID = ding->ID();
	this->m_dingName = ding->Name();
	this->m_selectedDingImage = dingImage;
	this->m_textLayout = nullptr;
	auto intrinsicFacing = ding->AvailableFacings();
	this->m_isSelectedDingRotatable = intrinsicFacing != Facings::Shy;
	if (resetFacing)
	{
		if (intrinsicFacing == Facings::Center)
		{
			/* this is actually the pipes, and their standard orientation is horz */
			this->m_selectedDingFacing = Facings::WE;
		}
		else 
		{
			/* clumsy packs work with initial shyness */
			this->m_selectedDingFacing = Facings::Shy;
		}
	}
}

void LevelEditorHUD::CreateTextLayout()
{
	if (this->m_textLayout == nullptr && this->IsDingSelected())
	{
		auto dwriteFactory = UserInterface::GetDWriteFactory();
		this->m_textStyle.SetFontName(L"Fredoka One");
		DX::ThrowIfFailed(
			dwriteFactory->CreateTextLayout(
				this->m_dingName->Data(),
				this->m_dingName->Length(),
				this->m_textStyle.GetTextFormat(),
				this->m_container.right - this->m_container.left,
				this->m_container.bottom - this->m_container.top,
				&this->m_textLayout
			)
		);

		this->m_textColorBrush->SetColor(ColorF(ColorF::Aquamarine));
	}
}

void LevelEditorHUD::Update(float timeTotal, float timeDelta)
{
	

}

void LevelEditorHUD::Render()
{
	auto d2dContext = UserInterface::GetD2DContext();
	D2D1_RECT_F bounds = this->GetBounds();
	d2dContext->FillRectangle(&bounds, this->m_shadowColorBrush.Get());

	if (this->m_selectedDingID > Dings::DingIDs::Void)
	{
		auto dingPic = this->m_selectedDingImage.Get();
		auto placementRect = D2D1::RectF(
			bounds.left + blooDot::Consts::DIALOG_PADDING,
			bounds.top + blooDot::Consts::DIALOG_PADDING,
			bounds.left + blooDot::Consts::DIALOG_PADDING + blooDot::Consts::SQUARE_WIDTH,
			bounds.top + blooDot::Consts::DIALOG_PADDING + blooDot::Consts::SQUARE_HEIGHT
		);

		d2dContext->DrawBitmap(dingPic, placementRect);
		d2dContext->DrawTextLayout(
			Point2F(
				bounds.left + blooDot::Consts::DIALOG_PADDING,
				bounds.top + blooDot::Consts::DIALOG_PADDING / 2.0f + blooDot::Consts::SQUARE_HEIGHT + blooDot::Consts::DIALOG_PADDING
			),
			this->m_textLayout.Get(),
			this->m_textColorBrush.Get(),
			D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
		);
	}
}

void LevelEditorHUD::ReleaseDeviceDependentResources()
{
	/*this->m_textColorBrush.Reset();
	this->m_shadowColorBrush.Reset();
	this->m_selectedDingImage.Reset();*/
}

Dings::DingIDs LevelEditorHUD::SelectedDingID()
{
	return this->m_selectedDingID;
}

void LevelEditorHUD::ToggleEraser()
{
	this->m_isEraserChosen = !this->m_isEraserChosen;
}

void LevelEditorHUD::ToggleOverwrite()
{
	this->m_isInOverwriteMode = !this->m_isInOverwriteMode;
}

void LevelEditorHUD::ToggleGrid()
{
	this->m_isGridShown = !this->m_isGridShown;
}

void LevelEditorHUD::ToggleDingSheet()
{
	this->m_isDingSheetShown = !this->m_isDingSheetShown;
}

void LevelEditorHUD::SetScrollLock(bool scrollLock)
{
	this->m_isScrollLocked = scrollLock;
}

bool LevelEditorHUD::IsInEraserMode()
{
	return this->m_isEraserChosen;
}

bool LevelEditorHUD::IsInOverwriteMode()
{
	return this->m_isInOverwriteMode;
}

bool LevelEditorHUD::IsGridShown()
{
	return this->m_isGridShown;
}

bool LevelEditorHUD::IsDingSheetShown()
{
	return this->m_isDingSheetShown;
}

bool LevelEditorHUD::IsScrollLocked()
{
	return this->m_isScrollLocked;
}

void LevelEditorHUD::Rotate()
{
	if (this->m_selectedDingID > Dings::DingIDs::Void && this->m_isSelectedDingRotatable)
	{
		this->m_selectedDingFacing = Dings::RotateFromFacing(this->m_selectedDingFacing, false);
	}
}

void LevelEditorHUD::RotateInverse()
{
	if (this->m_selectedDingID > Dings::DingIDs::Void && this->m_isSelectedDingRotatable)
	{
		this->m_selectedDingFacing = Dings::RotateFromFacing(this->m_selectedDingFacing, true);
	}
}

Facings LevelEditorHUD::SelectedDingOrientation()
{
	return this->m_selectedDingFacing;
}
