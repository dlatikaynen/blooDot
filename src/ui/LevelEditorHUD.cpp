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
	this->m_pendingDingSheetCommand = blooDot::DialogCommand::None;
}

void LevelEditorHUD::Initialize(std::shared_ptr<BrushRegistry> brushRegistry)
{
	this->m_brushRegistry = brushRegistry;
}

void LevelEditorHUD::CalculateSize()
{
	auto d2dContext = UserInterface::GetD2DContext();
	this->m_size = SizeF(
		d2dContext->GetSize().width / 2.0F,
		blooDot::Consts::SQUARE_HEIGHT * 1.75F
	);

	this->CreateTextLayout();
}

bool LevelEditorHUD::IsDingSelected()
{
	return this->m_selectedDingID > Dings::DingIDs::Void;
}

void LevelEditorHUD::SelectDing(std::shared_ptr<Dings> ding, Microsoft::WRL::ComPtr<ID2D1Bitmap> dingImage, bool resetFacing)
{
	this->m_selectedDingID = ding->ID();
	this->m_selectedDingExtent = ding->GetExtentOnSheet();
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
		this->m_textStyle.SetFontWeight(DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_LIGHT);
		this->m_textStyle.SetFontSize(15.f);
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
	}
}

void LevelEditorHUD::Update(float timeTotal, float timeDelta)
{
	

}

void LevelEditorHUD::Render()
{
	auto d2dContext = UserInterface::GetD2DContextWrapped();
	auto alphaRatio = static_cast<BYTE>(static_cast<float>(255) * blooDot::Consts::GOLDEN_RATIO);
	D2D1_RECT_F bounds = this->GetBounds();
	auto textColorBrush = this->m_brushRegistry->WannaHave(d2dContext, MFARGB{ 0xd4, 0xff, 0x7f, 255 }); // Aquamarine
	auto shadowColorBrush = this->m_brushRegistry->WannaHave(d2dContext, MFARGB{ 0, 0, 0, alphaRatio });
	auto shadowFrameBrush = this->m_brushRegistry->WannaHave(d2dContext, MFARGB{ 192, 192, 192, alphaRatio });
	d2dContext->FillRectangle(&bounds, shadowColorBrush.Get());
	d2dContext->DrawRectangle(&bounds, shadowFrameBrush.Get(), 1.3f);
	if (this->m_selectedDingID > Dings::DingIDs::Void)
	{
		auto dingPic = this->m_selectedDingImage.Get();
		auto placementRect = D2D1::RectF(
			bounds.left + blooDot::Consts::DIALOG_PADDING,
			bounds.top + blooDot::Consts::DIALOG_PADDING,
			bounds.left + blooDot::Consts::DIALOG_PADDING + blooDot::Consts::SQUARE_WIDTH,
			bounds.top + blooDot::Consts::DIALOG_PADDING + blooDot::Consts::SQUARE_HEIGHT
		);		

		auto originRect = D2D1::RectF(
			0.f,
			0.f,
			this->m_selectedDingExtent.width * blooDot::Consts::SQUARE_WIDTH,
			this->m_selectedDingExtent.height * blooDot::Consts::SQUARE_HEIGHT
		);

		auto interpolationMode = (this->m_selectedDingExtent.width == 1 && this->m_selectedDingExtent.height == 1) ? D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR : D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
		d2dContext->DrawBitmap(dingPic, placementRect, 1.f, interpolationMode, originRect);
		d2dContext->DrawTextLayout(
			Point2F(
				bounds.left + blooDot::Consts::DIALOG_PADDING,
				bounds.top + blooDot::Consts::DIALOG_PADDING / 2.0f + blooDot::Consts::SQUARE_HEIGHT + blooDot::Consts::DIALOG_PADDING
			),
			this->m_textLayout.Get(),
			textColorBrush.Get(),
			D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
		);
	}
}

void LevelEditorHUD::ReleaseDeviceDependentResources()
{
	/*
	this->m_selectedDingImage.Reset();
	*/
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

void LevelEditorHUD::SetDingSheetCommand(blooDot::DialogCommand dialogCommand)
{
	if (this->m_pendingDingSheetCommand == blooDot::DialogCommand::None)
	{
		this->m_pendingDingSheetCommand = dialogCommand;
	}
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

blooDot::DialogCommand LevelEditorHUD::DequeDingSheetCommand()
{
	auto pendingCommand = this->m_pendingDingSheetCommand;
	if (this->m_pendingDingSheetCommand != blooDot::DialogCommand::None)
	{
		this->m_pendingDingSheetCommand = blooDot::DialogCommand::None;
	}

	return pendingCommand;
}