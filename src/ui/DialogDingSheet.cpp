#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"
#include "DialogDingSheet.h"

DialogDingSheet::DialogDingSheet()
{
}

void DialogDingSheet::SetContent(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> dingSheet)
{
	this->m_dingSheet = dingSheet;
	this->CalculateSize();
}

void DialogDingSheet::RenderClientarea(ID2D1DeviceContext* d2dContext)
{
	DialogOverlay::RenderClientarea(d2dContext);


}
