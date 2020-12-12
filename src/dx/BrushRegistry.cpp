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

Microsoft::WRL::ComPtr<ID2D1Brush> BrushRegistry::WannaHave(Microsoft::WRL::ComPtr<ID2D1DeviceContext> dxDC, MFARGB color)
{
	Microsoft::WRL::ComPtr<ID2D1Brush> brush;
	if (this->m_Registry.size() != 0)
	{	
		brush = this->m_Registry[color];
	}

	if (brush == nullptr) 
	{
		auto brushColor = D2D1::ColorF(
			static_cast<float>(color.rgbRed) / 255.0F,
			static_cast<float>(color.rgbGreen) / 255.0F,
			static_cast<float>(color.rgbBlue) / 255.0F,
			static_cast<float>(color.rgbAlpha) / 255.0F
		);

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> specificBrush;
		DX::ThrowIfFailed(
			dxDC->CreateSolidColorBrush(brushColor, &specificBrush)
		);

		specificBrush->QueryInterface<ID2D1Brush>(&brush);
		this->m_Registry[color] = brush;
	}

	return brush;
}

Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> BrushRegistry::Rather(Microsoft::WRL::ComPtr<ID2D1DeviceContext> dxDC, MFARGB color, BYTE gradientAlpha2)
{
	Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> specificBrush;
	Microsoft::WRL::ComPtr<ID2D1Brush> genericBrush;
	if (this->m_Registry.size() != 0)
	{
		genericBrush = this->m_Registry[color];
	}

	if (genericBrush == nullptr)
	{
		auto brushColor1 = D2D1::ColorF(
			static_cast<float>(color.rgbRed) / 255.0F,
			static_cast<float>(color.rgbGreen) / 255.0F,
			static_cast<float>(color.rgbBlue) / 255.0F,
			static_cast<float>(color.rgbAlpha) / 255.0F
		);

		auto brushColor2 = D2D1::ColorF(
			static_cast<float>(color.rgbRed) / 255.0F,
			static_cast<float>(color.rgbGreen) / 255.0F,
			static_cast<float>(color.rgbBlue) / 255.0F,
			static_cast<float>(gradientAlpha2) / 255.0F
		);

		D2D1_GRADIENT_STOP gradientStops[2];
		gradientStops[0].color = brushColor1;
		gradientStops[0].position = 0.0f;
		gradientStops[1].color = brushColor2;
		gradientStops[1].position = 1.0f;
		ID2D1GradientStopCollection* pGradientStops = nullptr;
		DX::ThrowIfFailed(
			dxDC->CreateGradientStopCollection(
				gradientStops,
				2,
				D2D1_GAMMA_2_2,
				D2D1_EXTEND_MODE_CLAMP,
				&pGradientStops
			)
		);

		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gradientProperties = {};
		DX::ThrowIfFailed(
			dxDC->CreateLinearGradientBrush(gradientProperties, pGradientStops, &specificBrush)
		);

		this->m_Registry[color] = specificBrush;
	}
	else
	{
		genericBrush->QueryInterface<ID2D1LinearGradientBrush>(&specificBrush);
	}

	return specificBrush;
}

Microsoft::WRL::ComPtr<ID2D1Brush> BrushRegistry::WannaHave(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> dxTarget, MFARGB color)
{
	Microsoft::WRL::ComPtr<ID2D1Brush> brush;
	if (this->m_Registry.size() != 0)
	{
		brush = this->m_Registry[color];
	}

	if (brush == nullptr)
	{				
		auto brushColor = D2D1::ColorF(
			static_cast<float>(color.rgbRed) / 255.0F,
			static_cast<float>(color.rgbGreen) / 255.0F,
			static_cast<float>(color.rgbBlue) / 255.0F,
			static_cast<float>(color.rgbAlpha) / 255.0F
		);

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> specificBrush;
		DX::ThrowIfFailed(
			dxTarget->CreateSolidColorBrush(brushColor, &specificBrush)
		);

		specificBrush->QueryInterface<ID2D1Brush>(&brush);
		this->m_Registry[color] = brush;
	}

	return brush;
}

void BrushRegistry::Reset()
{
	this->m_Registry.clear();
}
