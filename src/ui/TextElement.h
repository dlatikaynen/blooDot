#pragma once

class TextElement : public ElementBase
{
public:
	TextElement();

	virtual void Initialize();
	virtual void Update(float timeTotal, float timeDelta);
	virtual void Render();
	virtual void ReleaseDeviceDependentResources();

	void SetTextColor(const D2D1_COLOR_F& textColor);
	void SetTextOpacity(float textOpacity);

	void SetText(__nullterminated WCHAR* text);
	void SetText(Platform::String^ text);

	TextStyle& GetTextStyle() { return m_textStyle; }

	void FadeOut(float fadeOutTime);

protected:
	virtual void CalculateSize();

	Platform::String^   m_text;
	D2D1_RECT_F         m_textExtents;
	TextStyle           m_textStyle;
	float               m_fadeStartingOpacity;
	float               m_fadeOutTime;
	float               m_fadeOutTimeElapsed;

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_textColorBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_shadowColorBrush;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>       m_textLayout;

	void CreateTextLayout();
};