#pragma once

class LevelEditorHUD : public ElementBase
{
public:
	LevelEditorHUD();

	virtual void Initialize();
	virtual void Update(float timeTotal, float timeDelta);
	virtual void Render();
	virtual void ReleaseDeviceDependentResources();

	bool IsDingSelected();
	void SelectDing(std::shared_ptr<Dings> ding, Microsoft::WRL::ComPtr<ID2D1Bitmap> dingImage, bool resetFacing);

	Dings::DingIDs SelectedDingID();
	void ToggleEraser();
	bool IsInEraserMode();
	void ToggleOverwrite();
	bool IsInOverwriteMode();
	void ToggleGrid();
	void ToggleDingSheet();
	void SetScrollLock(bool scrollLock);
	void SetDingSheetCommand(blooDot::DialogCommand dialogCommand);
	void Rotate();
	void RotateInverse();
	bool LevelEditorHUD::IsGridShown();
	bool LevelEditorHUD::IsDingSheetShown();
	bool LevelEditorHUD::IsScrollLocked();
	Facings LevelEditorHUD::SelectedDingOrientation();
	blooDot::DialogCommand DequeDingSheetCommand();

protected:
	virtual void CalculateSize();

	float											m_fadeStartingOpacity;
	float											m_fadeOutTime;
	float											m_fadeOutTimeElapsed;

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_shadowColorBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_textColorBrush;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>       m_textLayout;
	D2D1_RECT_F										m_textExtents;
	TextStyle										m_textStyle;

private:
	void CreateTextLayout();

	Dings::DingIDs									m_selectedDingID;
	Platform::String^								m_dingName;
	bool											m_isEraserChosen;
	bool											m_isInOverwriteMode;
	bool											m_isGridShown;
	bool											m_isDingSheetShown;
	bool											m_isScrollLocked;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>				m_selectedDingImage;
	bool											m_isSelectedDingRotatable;
	Facings											m_selectedDingFacing;
	D2D1_SIZE_U										m_selectedDingExtent;
	blooDot::DialogCommand							m_pendingDingSheetCommand;
};