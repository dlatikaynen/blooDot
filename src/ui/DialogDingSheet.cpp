#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"
#include "DialogDingSheet.h"

DialogDingSheet::DialogDingSheet()
{
}

void DialogDingSheet::SetContent(ID2D1Bitmap* dingSheetBitmap)
{	
	this->m_dingSheetBitmap = dingSheetBitmap;
	this->SetClientareaSize(this->m_dingSheetBitmap->GetSize());
	this->CalculateSize();
}

void DialogDingSheet::RenderClientarea(ID2D1DeviceContext* d2dContext)
{
	DialogOverlay::RenderClientarea(d2dContext);	
	auto dingSheetSize = this->m_dingSheetBitmap->GetSize();
	auto screenSize = d2dContext->GetSize();
	auto screenRect = D2D1::RectF(0.0F, 0.0F, screenSize.width - 1.0F, screenSize.height - 1.0F);
	auto xCenterOffset = screenSize.width / 2.0F - dingSheetSize.width / 2.0F;
	auto yCenterOffset = screenSize.height / 2.0F - dingSheetSize.height / 2.0F;
	auto sourceRect = D2D1::RectF(0.0F, 0.0F, dingSheetSize.width - 1.0F, dingSheetSize.height - 1.0F);
	auto dingRect = D2D1::RectF(xCenterOffset, yCenterOffset, xCenterOffset + dingSheetSize.width - 1.0F, yCenterOffset + dingSheetSize.height - 1.0F);
	auto windowRect = D2D1::RectF(dingRect.left - 10.0F, dingRect.top - 10.0F, dingRect.right + 10.0F, dingRect.bottom + 10.0F);
	//d2dContext->FillRectangle(screenRect, this->m_coverBrush.Get());
	//d2dContext->FillRectangle(windowRect, this->m_chromeBrush.Get());
	
	//d2dContext->FillRectangle(dingRect, this->m_blackBrush.Get());
	
	//d2dContext->DrawRectangle(windowRect, this->m_blackBrush.Get());
	
	d2dContext->DrawBitmap(
		this->m_dingSheetBitmap,
		dingRect,
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		sourceRect
	);
}
