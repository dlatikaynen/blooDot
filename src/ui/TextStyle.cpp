#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

TextStyle::TextStyle()
{
	m_fontName = L"Segoe UI";
	m_fontSize = 24.0f;
	m_fontWeight = DWRITE_FONT_WEIGHT_NORMAL;
	m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
	m_textAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
}

void TextStyle::SetFontName(Platform::String^ fontName)
{
	if (!m_fontName->Equals(fontName))
	{
		m_fontName = fontName;
		m_textFormat = nullptr;
	}
}

void TextStyle::SetFontSize(float fontSize)
{
	if (m_fontSize != fontSize)
	{
		m_fontSize = fontSize;
		m_textFormat = nullptr;
	}
}

void TextStyle::SetFontWeight(DWRITE_FONT_WEIGHT fontWeight)
{
	if (m_fontWeight != fontWeight)
	{
		m_fontWeight = fontWeight;
		m_textFormat = nullptr;
	}
}

void TextStyle::SetFontStyle(DWRITE_FONT_STYLE fontStyle)
{
	if (m_fontStyle != fontStyle)
	{
		m_fontStyle = fontStyle;
		m_textFormat = nullptr;
	}
}

void TextStyle::SetTextAlignment(DWRITE_TEXT_ALIGNMENT textAlignment)
{
	if (m_textAlignment != textAlignment)
	{
		m_textAlignment = textAlignment;
		m_textFormat = nullptr;
	}
}

IDWriteTextFormat* TextStyle::GetTextFormat()
{
	if (m_textFormat == nullptr)
	{
		IDWriteFactory* dwriteFactory = UserInterface::GetDWriteFactory();

		DX::ThrowIfFailed(
			dwriteFactory->CreateTextFormat(
				m_fontName->Data(),
				nullptr,
				m_fontWeight,
				m_fontStyle,
				DWRITE_FONT_STRETCH_NORMAL,
				m_fontSize,
				L"en-US",
				&m_textFormat
			)
		);

		DX::ThrowIfFailed(
			m_textFormat->SetTextAlignment(m_textAlignment)
		);

		DX::ThrowIfFailed(
			m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
		);
	}

	return m_textFormat.Get();
}