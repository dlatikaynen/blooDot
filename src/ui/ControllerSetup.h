#pragma once

class ControllerSetup : public ElementBase
{
public:
	ControllerSetup();

	virtual void Initialize();
	virtual void Update(float timeTotal, float timeDelta);
	virtual void Render();
	virtual void ReleaseDeviceDependentResources();

protected:
	virtual void CalculateSize();

	Platform::String^   m_text;
	D2D1_RECT_F         m_textExtents;
	TextStyle           m_textStyle;
	float               m_fadeStartingOpacity;
	float               m_fadeOutTime;
	float               m_fadeOutTimeElapsed;

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	m_borderBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	m_textColorBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	m_selectionBackdropBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	m_shadowColorBrush;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>		m_textLayout;

	void CreateTextLayout();
};