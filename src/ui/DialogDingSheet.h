#pragma once

class DialogDingSheet : public DialogOverlay
{
public:
	DialogDingSheet();
	virtual Platform::String^ StaticCaption() override { return L"Ding Sheet"; }
	void SetContent(ID2D1Bitmap* dingSheetBitmap);

protected:
	void RenderClientarea(ID2D1DeviceContext* d2dContext) override;

private:	
	ID2D1Bitmap* m_dingSheetBitmap;
};