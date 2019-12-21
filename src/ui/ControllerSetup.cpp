#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

ControllerSetup::ControllerSetup()
{
}

void ControllerSetup::Initialize()
{
	auto d2dContext = UserInterface::GetD2DContext();
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::White), &m_textColorBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::White, 0.2f), &m_selectionBackdropBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::Black), &m_shadowColorBrush));
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(ColorF(ColorF::White, 0.5f), &m_borderBrush));
}

void ControllerSetup::Update(float timeTotal, float timeDelta)
{

}

void ControllerSetup::Render()
{
	auto g = UserInterface::GetD2DContext();
	auto bounds = GetBounds();
	D2D1_POINT_2F origin = Point2F(
		bounds.left - m_textExtents.left,
		bounds.top - m_textExtents.top
	);

	auto layoutRect = D2D1::RoundedRect(D2D1::RectF(10, 100, 230, 70), 4.0f, 4.0f);
	g->DrawRoundedRectangle(layoutRect, this->m_borderBrush.Get());
}

void ControllerSetup::ReleaseDeviceDependentResources()
{
	m_textColorBrush.Reset();
	m_selectionBackdropBrush.Reset();
	m_shadowColorBrush.Reset();
	m_borderBrush.Reset();
}

void ControllerSetup::CalculateSize()
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

void ControllerSetup::CreateTextLayout()
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