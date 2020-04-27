#pragma once

class DialogDingSheet : public DialogOverlay
{
public:
	DialogDingSheet();

	virtual void Initialize() override;
	virtual Platform::String^ StaticCaption() override { return L"der b�se Kurt"; }
	void SetContent(std::shared_ptr<Level> levelInfo);
	virtual void ReleaseDeviceDependentResources() override;

	virtual void Update(float timeTotal, float timeDelta) override;


protected:
	void RenderClientarea(ID2D1DeviceContext* d2dContext) override;

private:	
	ID2D1Bitmap*									m_dingSheetBitmap;
	std::shared_ptr<Level>							m_LevelInfo;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>m_boundsBrush;
	float der_boese_kurtx;
	float der_boese_kurty;
};
