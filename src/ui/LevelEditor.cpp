#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

LevelEditor::LevelEditor()
{
}

LevelEditor::~LevelEditor()
{
}

void LevelEditor::Update(float timeTotal, float timeDelta)
{
	WorldScreenBase::Update(timeTotal, timeDelta);
	if (m_isMoving & Facings::East)
	{
		m_viewportOffset = D2D1::Point2F(m_viewportOffset.x + 2.0, m_viewportOffset.y);
	}
	else if (m_isMoving & Facings::West)
	{
		m_viewportOffset = D2D1::Point2F(m_viewportOffset.x - 2.0, m_viewportOffset.y);
	}

	if (m_isMoving & Facings::South)
	{
		m_viewportOffset = D2D1::Point2F(m_viewportOffset.x, m_viewportOffset.y + 2.0);
	}
	else if (m_isMoving & Facings::North)
	{
		m_viewportOffset = D2D1::Point2F(m_viewportOffset.x, m_viewportOffset.y - 2.0);
	}
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
	D2D1_RECT_F rect0;
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

	auto brusherl = brush.Get();

	/* the next multiple of the grid height less than the viewport top */
	auto yAdjust = static_cast<float>(static_cast<unsigned>(m_viewportOffset.y) % static_cast<unsigned>(SQUARE_HEIGHT));
	auto xAdjust = static_cast<float>(static_cast<unsigned>(m_viewportOffset.x) % static_cast<unsigned>(SQUARE_WIDTH));

	for (int y = -yAdjust; y < static_cast<int>(m_viewportSize.height+SQUARE_HEIGHT); y += static_cast<int>(SQUARE_HEIGHT))
	{
		point0.x = 0;
		point0.y = y;
		point1.x = static_cast<int>(m_viewportSize.width);
		point1.y = y;
		m_d2dContext->DrawLine(point0, point1, brusherl, 1.0F, NULL);
	}

	for (int x = -xAdjust; x < static_cast<int>(m_viewportSize.width+SQUARE_WIDTH); x += static_cast<int>(SQUARE_WIDTH))
	{
		point0.x = x;
		point0.y = 0;
		point1.x = x;
		point1.y = static_cast<int>(m_viewportSize.height);
		m_d2dContext->DrawLine(point0, point1, brusherl, 1.0F, NULL);
	}

	rect0.left = 0.0;
	rect0.top = 0.0;
	rect0.right = rect0.left + SQUARE_WIDTH;
	rect0.bottom = rect0.top + SQUARE_HEIGHT;

	m_d2dContext->DrawRectangle(rect0, highlight.Get(), 1.0F, NULL);
}