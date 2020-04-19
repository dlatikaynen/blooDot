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
}

void DialogDingSheet::RenderClientarea(ID2D1DeviceContext* d2dContext)
{
	DialogOverlay::RenderClientarea(d2dContext);	
	auto dingSheetSize = this->m_dingSheetBitmap->GetSize();
	auto sourceRect = D2D1::RectF(0.0F, 0.0F, dingSheetSize.width - 1.0F, dingSheetSize.height - 1.0F);
	d2dContext->DrawBitmap(
		this->m_dingSheetBitmap,
		this->m_clientArea,
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		sourceRect
	);
}
