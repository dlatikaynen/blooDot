#pragma once

class DialogDingSheet : public DialogOverlay
{
public:
	DialogDingSheet();
	virtual Platform::String^ StaticCaption() override { return L"Ding Sheet"; }
	void SetContent(std::shared_ptr<Level> levelInfo);

protected:
	void RenderClientarea(ID2D1DeviceContext* d2dContext) override;

private:	
	ID2D1Bitmap*			m_dingSheetBitmap;
	std::shared_ptr<Level>	m_LevelInfo;
};