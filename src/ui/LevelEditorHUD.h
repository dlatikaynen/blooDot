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
	void SelectDing(Dings* ding, Microsoft::WRL::ComPtr<ID2D1Bitmap> dingImage);

	unsigned SelectedDingID();
	void ToggleEraser(bool doErase);
	bool IsInEraserMode();

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

	unsigned										m_selectedDingID;
	Platform::String^								m_dingName;
	bool											m_isEraserChosen;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>				m_selectedDingImage;
};