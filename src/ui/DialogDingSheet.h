#pragma once

class DialogDingSheet : public DialogOverlay
{
public:
	DialogDingSheet();

	virtual void Initialize(std::shared_ptr<BrushRegistry> brushRegistry) override;
	virtual Platform::String^ StaticCaption() override;
	void SetContent(std::shared_ptr<Level> levelInfo);
	virtual void ReleaseDeviceDependentResources() override;

protected:
	void Update(float timeTotal, float timeDelta) override;
	void RenderClientarea(ID2D1DeviceContext* d2dContext) override;

	enum Pane 
	{
		Dings,
		Mobs
	};

private:
	void TogglePane();

	ID2D1Bitmap*									m_dingSheetBitmap;
	std::shared_ptr<Level>							m_LevelInfo;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	m_boundsBrush;
	Pane											m_currentPane;
};
