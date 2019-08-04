//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "LoadScreen.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;

void LoadScreen::Initialize(
    _In_ ID2D1Device*         d2dDevice,
    _In_ ID2D1DeviceContext*  d2dContext,
	_In_ std::shared_ptr<DX::DeviceResources>& deviceResources,
    _In_ IWICImagingFactory*  wicFactory
    )
{
	m_deviceResources = deviceResources,
    m_wicFactory = wicFactory;
    m_d2dDevice = d2dDevice;
    m_d2dContext = d2dContext;
    m_imageSize = D2D1::SizeF(0.0f, 0.0f);
    m_offset = D2D1::SizeF(0.0f, 0.0f);
	m_moved = D2D1::SizeF(0.0f, 0.0f);
	m_totalSize = D2D1::SizeF(0.0f, 0.0f);

    ComPtr<ID2D1Factory> factory;
    d2dDevice->GetFactory(&factory);

    DX::ThrowIfFailed(
        factory.As(&m_d2dFactory)
        );

	srand(time(NULL));

	for (int i = 0; i < 100; ++i) for (int j = 0; j < 60; ++j) m_GoL[i][j] = !((rand() % (rand() % 4 + 1)) == 0);
	
	CreateDeviceDependentResources();
	ResetDirectXResources();
}

void LoadScreen::CreateDeviceDependentResources()
{
	DX::ThrowIfFailed(
		m_d2dContext->CreateSolidColorBrush(ColorF(ColorF::Goldenrod), &m_GoLBrush)
	);
}

void LoadScreen::ResetDirectXResources()
{
    ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
    DX::ThrowIfFailed(
        m_wicFactory->CreateDecoderFromFilename(
            L"Media\\Textures\\loadscreen.png",
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnDemand,
            &wicBitmapDecoder
            )
        );

    ComPtr<IWICBitmapFrameDecode> wicBitmapFrame;
    DX::ThrowIfFailed(
        wicBitmapDecoder->GetFrame(0, &wicBitmapFrame)
        );

    ComPtr<IWICFormatConverter> wicFormatConverter;
    DX::ThrowIfFailed(
        m_wicFactory->CreateFormatConverter(&wicFormatConverter)
        );

    DX::ThrowIfFailed(
        wicFormatConverter->Initialize(
            wicBitmapFrame.Get(),
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom  // the BGRA format has no palette so this value is ignored
            )
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateBitmapFromWicBitmap(
            wicFormatConverter.Get(),
            BitmapProperties(PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            &m_bitmap
            )
        );

    m_imageSize = m_bitmap->GetSize();

    DX::ThrowIfFailed(
        m_d2dFactory->CreateDrawingStateBlock(&m_stateBlock)
        );

    UpdateForWindowSizeChange();
}

void LoadScreen::ReleaseDeviceDependentResources()
{
    m_d2dDevice.Reset();
    m_d2dContext.Reset();
    m_bitmap.Reset();
    m_d2dFactory.Reset();
    m_stateBlock.Reset();
    m_wicFactory.Reset();
	m_GoLBrush.Reset();
	m_moved.width = m_moved.height = 0;
}

void LoadScreen::UpdateForWindowSizeChange()
{
    Windows::Foundation::Rect windowBounds = CoreWindow::GetForCurrentThread()->Bounds;

    m_offset.width = (windowBounds.Width - m_imageSize.width) / 2.0f;
    m_offset.height = (windowBounds.Height - m_imageSize.height) / 2.0f;

    m_totalSize.width = m_offset.width + m_imageSize.width;
    m_totalSize.height = m_offset.height + m_imageSize.height;
}

void LoadScreen::Update(float timeTotal, float timeDelta)
{
	//m_moved.width++;

	/*
		Births: Each dead cell adjacent to exactly three live neighbors will become live in the next generation.
		Death by isolation: Each live cell with one or fewer live neighbors will die in the next generation.
		Death by overcrowding: Each live cell with four or more live neighbors will die in the next generation.
		Survival: Each live cell with either two or three live neighbors will remain alive for the next generation.
	*/
	for (int i = 0; i < 100; ++i) for (int j = 0; j < 60; ++j)
	{
		bool curState = m_GoL[i][j];
		int NAl = NeighborsAlive(i, j);
		if (curState)
		{
			if (NAl == 2 || NAl == 3)
			{
				/* survives */
				m_GoL2[i][j] = true;
			}
			else if (NAl < 2)
			{
				/* starvation */
				m_GoL2[i][j] = false;
			}
			else if (NAl > 3)
			{
				/* overcrowded */
				m_GoL2[i][j] = false;
			}
		}
		else
		{
			/* birth */
			m_GoL2[i][j] = NAl == 3;
		}
	}

	/* and then the rain set in */
	int k = 0;
	for (int z = 0; z < 100; ++z) {
		int i = rand() % 100;
		int j = rand() % 60;
		if (!m_GoL2[i][j])
		{
			m_GoL2[i][j] = true;
			++k;
		}
		if (k == 9) break;
	}

	std::copy(&m_GoL2[0][0], &m_GoL2[0][0]+100*60, &m_GoL[0][0]);
}

bool LoadScreen::NeighborN(int i, int j)
{
	return m_GoL[i][IndexUp(j)];
}

bool LoadScreen::NeighborS(int i, int j)
{
	return m_GoL[i][IndexDown(j)];
}

bool LoadScreen::NeighborE(int i, int j)
{
	return m_GoL[IndexRight(i)][j];
}

bool LoadScreen::NeighborNE(int i, int j)
{
	return m_GoL[IndexRight(i)][IndexUp(j)];
}

bool LoadScreen::NeighborSE(int i, int j)
{
	return m_GoL[IndexRight(i)][IndexDown(j)];
}

bool LoadScreen::NeighborW(int i, int j)
{
	return m_GoL[IndexLeft(i)][j];
}

bool LoadScreen::NeighborSW(int i, int j)
{
	return m_GoL[IndexLeft(i)][IndexDown(j)];
}

bool LoadScreen::NeighborNW(int i, int j)
{
	return m_GoL[IndexLeft(i)][IndexUp(j)];
}

int LoadScreen::NeighborsAlive(int i, int j) 
{
	int na = 0
		+ (NeighborN(i, j) ? 1 : 0)
		+ (NeighborNE(i, j) ? 1 : 0)
		+ (NeighborE(i, j) ? 1 : 0)
		+ (NeighborSE(i, j) ? 1 : 0)
		+ (NeighborS(i, j) ? 1 : 0)
		+ (NeighborSW(i, j) ? 1 : 0)
		+ (NeighborW(i, j) ? 1 : 0)
		+ (NeighborNW(i, j) ? 1 : 0);
	return na;
}

int LoadScreen::IndexLeft(int i) 
{
	if (i == 0) return 99;
	return i - 1;
}

int LoadScreen::IndexRight(int i)
{
	if (i == 99) return 0;
	return i + 1;
}

int LoadScreen::IndexUp(int j) 
{
	if (j == 0) return 59;
	return j - 1;
}

int LoadScreen::IndexDown(int j)
{
	if (j == 59) return 0;
	return j + 1;
}

void LoadScreen::Render(D2D1::Matrix3x2F orientation2D)
{
    m_d2dContext->SaveDrawingState(m_stateBlock.Get());

    m_d2dContext->BeginDraw();
    m_d2dContext->SetTransform(orientation2D);

	//m_d2dContext->DrawBitmap(
 //       m_bitmap.Get(),
 //       D2D1::RectF(
 //           m_offset.width+m_moved.width,
 //           m_offset.height+m_moved.height,
 //           m_imageSize.width + m_offset.width+m_moved.width,
 //           m_imageSize.height + m_offset.height+m_moved.height
	//	)
 //   );

	for (int i = 0; i < 100; ++i) for (int j = 0; j < 60; ++j) if (m_GoL[i][j]) DrawCell(i, j);

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    m_d2dContext->RestoreDrawingState(m_stateBlock.Get());
}

void LoadScreen::DrawCell(int x, int y) 
{
	D2D1_RECT_F rect;
	rect.left = 11*x;
	rect.top = 11*y;
	rect.right = rect.left+10;
	rect.bottom = rect.top + 10;
	D2D1_ROUNDED_RECT rrect;
	rrect.rect = rect;
	rrect.radiusX = 2;
	rrect.radiusY = 2;

	m_d2dContext->FillRoundedRectangle(
		&rrect,
		m_GoLBrush.Get()
	);
}
