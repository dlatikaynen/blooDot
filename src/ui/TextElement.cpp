#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

TextElement::TextElement()
{
}

void TextElement::Initialize()
{
	ID2D1DeviceContext* d2dContext = UserInterface::GetD2DContext();
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::White), &m_textColorBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::White, 0.2f), &m_selectionBackdropBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::Black), &m_shadowColorBrush));
}

void TextElement::Update(float timeTotal, float timeDelta)
{
	if (m_isFadingOut)
	{
		m_fadeOutTimeElapsed += timeDelta;

		float delta = min(1.0f, m_fadeOutTimeElapsed / m_fadeOutTime);
		SetTextOpacity((1.0f - delta) * m_fadeStartingOpacity);

		if (m_fadeOutTimeElapsed >= m_fadeOutTime)
		{
			m_isFadingOut = false;
			SetVisible(false);
		}
	}
}

void TextElement::Render()
{
	ID2D1DeviceContext* d2dContext = UserInterface::GetD2DContext();

	D2D1_RECT_F bounds = GetBounds();
	D2D1_POINT_2F origin = Point2F(
		bounds.left - m_textExtents.left,
		bounds.top - m_textExtents.top
	);

	m_shadowColorBrush->SetOpacity(m_textColorBrush->GetOpacity() * 0.5f);

	d2dContext->DrawTextLayout(
		Point2F(origin.x + 4.0f, origin.y + 4.0f),
		m_textLayout.Get(),
		m_shadowColorBrush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
	);

	d2dContext->DrawTextLayout(
		origin,
		m_textLayout.Get(),
		m_textColorBrush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
	);
}

void TextElement::ReleaseDeviceDependentResources()
{
	/*m_textColorBrush.Reset();
	m_selectionBackdropBrush.Reset();
	m_shadowColorBrush.Reset();*/
}

void TextElement::SetTextColor(const D2D1_COLOR_F& textColor)
{
	m_textColorBrush->SetColor(textColor);
}

void TextElement::SetTextOpacity(float textOpacity)
{
	m_textColorBrush->SetOpacity(textOpacity);
}

void TextElement::SetText(__nullterminated WCHAR* text)
{
	SetText(ref new Platform::String(text));
}

void TextElement::SetText(Platform::String^ text)
{
	if (!m_text->Equals(text))
	{
		m_text = text;
		m_textLayout = nullptr;
	}
}

void TextElement::FadeOut(float fadeOutTime)
{
	if (m_visible)
	{
		m_fadeStartingOpacity = m_textColorBrush->GetOpacity();
		m_fadeOutTime = fadeOutTime;
		m_fadeOutTimeElapsed = 0.0f;
		m_isFadingOut = true;
	}
}

void TextElement::CalculateSize()
{
	CreateTextLayout();

	DWRITE_TEXT_METRICS metrics;
	DWRITE_OVERHANG_METRICS overhangMetrics;
	DX::ThrowIfFailed(
		m_textLayout->GetMetrics(&metrics)
	);
	DX::ThrowIfFailed(
		m_textLayout->GetOverhangMetrics(&overhangMetrics)
	);

	m_textExtents = RectF(
		-overhangMetrics.left,
		-overhangMetrics.top,
		overhangMetrics.right + metrics.layoutWidth,
		overhangMetrics.bottom + metrics.layoutHeight
	);

	m_size = SizeF(
		m_textExtents.right - m_textExtents.left,
		m_textExtents.bottom - m_textExtents.top
	);
}

void TextElement::CreateTextLayout()
{
	if ((m_textLayout == nullptr) || m_textStyle.HasTextFormatChanged())
	{
		m_textLayout = nullptr;
		IDWriteFactory* dwriteFactory = UserInterface::GetDWriteFactory();
		DX::ThrowIfFailed(
			dwriteFactory->CreateTextLayout(
				m_text->Data(),
				m_text->Length(),
				m_textStyle.GetTextFormat(),
				m_container.right - m_container.left,
				m_container.bottom - m_container.top,
				&m_textLayout
			)
		);
	}
}