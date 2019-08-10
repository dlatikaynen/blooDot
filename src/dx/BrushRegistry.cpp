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
}

Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> BrushRegistry::WannaHave(Microsoft::WRL::ComPtr<ID2D1DeviceContext> dxDC, MFARGB color)
{
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = m_Registry.at(color);
	if (brush == NULL) 
	{
		D2D1_COLOR_F brushColor = D2D1::ColorF(color.rgbRed, color.rgbGreen, color.rgbBlue, color.rgbAlpha);
		DX::ThrowIfFailed(
			dxDC->CreateSolidColorBrush(brushColor, &brush)
		);

		m_Registry.emplace(color, brush);
	}

	return brush;
}

void BrushRegistry::Reset()
{
	m_Registry.clear();
}
