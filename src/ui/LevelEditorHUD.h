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
	void SelectDing(unsigned dingID, Microsoft::WRL::ComPtr<ID2D1Bitmap> dingImage);

	unsigned SelectedDingID();
	void ToggleEraser();
	bool IsInEraserMode();

protected:
	virtual void CalculateSize();

	float											m_fadeStartingOpacity;
	float											m_fadeOutTime;
	float											m_fadeOutTimeElapsed;

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_textColorBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_shadowColorBrush;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>       m_textLayout;

private:
	unsigned										m_selectedDingID;
	bool											m_isEraserChosen;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>				m_selectedDingImage;
};