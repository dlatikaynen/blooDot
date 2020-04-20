#include "..\PreCompiledHeaders.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;
using namespace DirectX;

#include "DirectXHelper.h"
#include "BrushRegistry.h"

BrushRegistry::BrushRegistry()
{
}

BrushRegistry::~BrushRegistry()
{
	this->Reset();
}

Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> BrushRegistry::WannaHave(Microsoft::WRL::ComPtr<ID2D1DeviceContext> dxDC, MFARGB color)
{
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = NULL;
	if (m_Registry.size() != 0)
	{	
		brush = m_Registry[color];
	}

	if (brush == NULL) 
	{
		D2D1_COLOR_F brushColor = D2D1::ColorF(
			static_cast<float>(color.rgbRed) / 255.0F, 
			static_cast<float>(color.rgbGreen) / 255.0F,
			static_cast<float>(color.rgbBlue) / 255.0F,
			static_cast<float>(color.rgbAlpha) / 255.0F
		);

		DX::ThrowIfFailed(
			dxDC->CreateSolidColorBrush(brushColor, &brush)
		);

		m_Registry.emplace(color, brush);
	}

	return brush;
}

Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> BrushRegistry::WannaHave(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> dxTarget, MFARGB color)
{
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = NULL;
	if (m_Registry.size() != 0)
	{
		brush = m_Registry[color];
	}

	if (brush == NULL)
	{
		D2D1_COLOR_F brushColor = D2D1::ColorF(
			static_cast<float>(color.rgbRed) / 255.0F,
			static_cast<float>(color.rgbGreen) / 255.0F,
			static_cast<float>(color.rgbBlue) / 255.0F,
			static_cast<float>(color.rgbAlpha) / 255.0F
		);

		DX::ThrowIfFailed(
			dxTarget->CreateSolidColorBrush(brushColor, &brush)
		);

		m_Registry.emplace(color, brush);
	}

	return brush;
}

void BrushRegistry::Reset()
{
	m_Registry.clear();
}
