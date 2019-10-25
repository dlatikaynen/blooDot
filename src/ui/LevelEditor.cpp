#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

LevelEditor::LevelEditor()
{
}

LevelEditor::~LevelEditor()
{
}

void LevelEditor::Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition)
{
	m_d2dContext->SaveDrawingState(m_stateBlock.Get());
	m_d2dContext->BeginDraw();
	m_d2dContext->SetTransform(orientation2D);
	auto screenRect = D2D1::RectF(
		0,
		0,
		m_viewportSize.width,
		m_viewportSize.height
	);

	ID2D1Bitmap *bmp = NULL;
	m_floor->GetBitmap(&bmp);
	m_d2dContext->DrawBitmap(bmp, screenRect);
	bmp->Release();

	this->DrawLevelEditorRaster();

	m_walls->GetBitmap(&bmp);
	m_d2dContext->DrawBitmap(bmp, screenRect);
	bmp->Release();

	m_rooof->GetBitmap(&bmp);
	m_d2dContext->DrawBitmap(bmp, screenRect);
	bmp->Release();

	HRESULT hr = m_d2dContext->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
	{
		DX::ThrowIfFailed(hr);
	}

	m_d2dContext->RestoreDrawingState(m_stateBlock.Get());
}

void LevelEditor::DrawLevelEditorRaster()
{
	D2D1_POINT_2F point0, point1;
	MFARGB color;
	color.rgbAlpha = 128;
	color.rgbRed = 128;
	color.rgbGreen = 128;
	color.rgbBlue = 128;
	MFARGB colhigh;
	colhigh.rgbAlpha = 255;
	colhigh.rgbRed = 0;
	colhigh.rgbGreen = 0;
	colhigh.rgbBlue = 255;
	Microsoft::WRL::ComPtr<ID2D1Brush> brush = m_Brushes.WannaHave(m_d2dContext, color);
	Microsoft::WRL::ComPtr<ID2D1Brush> highlight = m_Brushes.WannaHave(m_d2dContext, colhigh);

	for (int y = 1; y < 1000; y += 49)
	{
		point0.x = 0;
		point0.y = y;
		point1.x = 1000;
		point1.y = y;
		m_d2dContext->DrawLine(point0, point1, y == 1 ? highlight.Get() : brush.Get(), 1.0F, NULL);
	}

	for (int x = 1; x < 1000; x += 49)
	{
		point0.x = x;
		point0.y = 0;
		point1.x = x;
		point1.y = 1000;
		m_d2dContext->DrawLine(point0, point1, x == 1 ? highlight.Get() : brush.Get(), 1.0F, NULL);
	}
}