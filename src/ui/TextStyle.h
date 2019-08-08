#pragma once

class TextStyle
{
public:
	TextStyle();

	void SetFontName(Platform::String^ fontName);
	void SetFontSize(float fontSize);
	void SetFontWeight(DWRITE_FONT_WEIGHT fontWeight);
	void SetFontStyle(DWRITE_FONT_STYLE fontStyle);
	void SetTextAlignment(DWRITE_TEXT_ALIGNMENT textAlignment);

	IDWriteTextFormat* GetTextFormat();

	bool HasTextFormatChanged() { return (m_textFormat == nullptr); }

private:
	Platform::String^       m_fontName;
	float                   m_fontSize;
	DWRITE_FONT_WEIGHT      m_fontWeight;
	DWRITE_FONT_STYLE       m_fontStyle;
	DWRITE_TEXT_ALIGNMENT   m_textAlignment;

	Microsoft::WRL::ComPtr<IDWriteTextFormat>   m_textFormat;
};

enum AlignType
{
	AlignNear,
	AlignCenter,
	AlignFar,
};

struct Alignment
{
	AlignType horizontal;
	AlignType vertical;
};
