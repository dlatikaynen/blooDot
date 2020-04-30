#pragma once

class DialogDingSheet : public DialogOverlay
{
public:
	DialogDingSheet();

	virtual void Initialize() override;
	virtual Platform::String^ StaticCaption() override { return L"Ding Sheet"; }
	void SetContent(std::shared_ptr<Level> levelInfo);
	virtual void ReleaseDeviceDependentResources() override;
	
protected:
	void RenderClientarea(ID2D1DeviceContext* d2dContext) override;

private:	
	ID2D1Bitmap*									m_dingSheetBitmap;
	std::shared_ptr<Level>							m_LevelInfo;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	m_boundsBrush;
};
