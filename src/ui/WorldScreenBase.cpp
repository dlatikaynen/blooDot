#include "..\PreCompiledHeaders.h"
#include "WorldScreenBase.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;

WorldScreenBase::WorldScreenBase() 
{
}

void WorldScreenBase::Initialize(_In_ std::shared_ptr<DX::DeviceResources>&	deviceResources)
{
	m_deviceResources = deviceResources;
	m_wicFactory = deviceResources->GetWicImagingFactory();
	m_d2dDevice = deviceResources->GetD2DDevice();
	m_d2dContext = deviceResources->GetD2DDeviceContext();
	m_backgroundSize = D2D1::SizeF(0.0f, 0.0f);
	m_viewportSize = D2D1::SizeU(0, 0);
	m_totalSize = D2D1::SizeF(0.0f, 0.0f);

    ComPtr<ID2D1Factory> factory;
    m_d2dDevice->GetFactory(&factory);

    DX::ThrowIfFailed(
        factory.As(&m_d2dFactory)
    );
	
	CreateDeviceDependentResources();
	ResetDirectXResources();
}

void WorldScreenBase::CreateDeviceDependentResources()
{

}

void WorldScreenBase::ResetDirectXResources()
{
    ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
    DX::ThrowIfFailed(
        m_wicFactory->CreateDecoderFromFilename(
            L"Media\\Bitmaps\\universe_seamless.png",
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
            WICBitmapPaletteTypeCustom
        )
    );

    DX::ThrowIfFailed(
        m_d2dContext->CreateBitmapFromWicBitmap(
            wicFormatConverter.Get(),
            BitmapProperties(PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            &m_background
        )
    );

    m_backgroundSize = m_background->GetSize();
	m_viewportSize.width = (unsigned int)m_backgroundSize.width;
	m_viewportSize.height = (unsigned int)m_backgroundSize.height;

	DX::ThrowIfFailed(
		m_d2dContext->CreateCompatibleRenderTarget(
			D2D1::SizeF(800.0f, 600.0f),
			&m_dings
		)
	);

	DX::ThrowIfFailed(
		m_d2dContext->CreateCompatibleRenderTarget(
			m_backgroundSize,
			&m_floor
		) 
	);

	DX::ThrowIfFailed(
		m_d2dContext->CreateCompatibleRenderTarget(
			m_backgroundSize,
			&m_walls
		)
	);

	DX::ThrowIfFailed(
		m_d2dContext->CreateCompatibleRenderTarget(
			m_backgroundSize,
			&m_rooof
		)
	);

    DX::ThrowIfFailed(
        m_d2dFactory->CreateDrawingStateBlock(&m_stateBlock)
    );

    UpdateForWindowSizeChange();
}

void WorldScreenBase::ReleaseDeviceDependentResources()
{
    m_d2dDevice.Reset();
    m_d2dContext.Reset();
    m_background.Reset();
	m_dings.Reset();
	m_floor.Reset();
	m_walls.Reset();
	m_rooof.Reset();	
    m_d2dFactory.Reset();
    m_stateBlock.Reset();
    m_wicFactory.Reset();
	m_Brushes.Reset();
}

void WorldScreenBase::UpdateForWindowSizeChange()
{
	m_isResizing = true;

    Windows::Foundation::Rect windowBounds = CoreWindow::GetForCurrentThread()->Bounds;

    m_totalSize.width = m_viewportSize.width;
    m_totalSize.height = m_viewportSize.height;
	D2D1_SIZE_F canvasSize = m_d2dContext->GetSize();

	/* pre-draw all dings */
	auto deflt = Dings(0, "BLACK", m_Brushes);
	auto mauer = Mauer(m_Brushes);

	m_dings->BeginDraw();

	deflt.Draw(m_dings, 0, 0);
	mauer.Draw(m_dings, 1, 1);

	DX::ThrowIfFailed(
		m_dings->EndDraw()
	);

	ID2D1Bitmap *dings = NULL;
	m_dings->GetBitmap(&dings);
	m_walls->BeginDraw();
	PlacePrimitive(dings, m_walls, 1, 1, 0, 0);
	PlacePrimitive(dings, m_walls, 0, 0, 1, 0);
	PlacePrimitive(dings, m_walls, 10, 5, 2, 0);
	PlacePrimitive(dings, m_walls, 10, 5, 3, 1);
	dings->Release();
	DX::ThrowIfFailed(
		m_walls->EndDraw()
	);

	m_isResizing = false;
}

void WorldScreenBase::PlacePrimitive(ID2D1Bitmap *dingSurface, Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> renderTarget, int dingX, int dingY, int placementX, int placementY)
{
	D2D1_RECT_F dingRect = D2D1::RectF(dingX * 50.0f, dingX * 50.0f, dingX * 50.0f + 49.0f, dingY * 50.0f + 49.0f);
	D2D1_RECT_F placementRect = D2D1::RectF(placementX * 50.0f, placementY * 50.0f, placementX * 50.0f + 49.0f, placementY * 50.0f + 49.0f);
	renderTarget->DrawBitmap(dingSurface, placementRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, dingRect);
}

void WorldScreenBase::Update(float timeTotal, float timeDelta)
{
	if (m_isResizing) 
	{
		return;
	}

	auto viewportRect = D2D1::RectF(
		0.0f,
		0.0f,
		m_viewportSize.width,
		m_viewportSize.height
	);

	auto copyOffset = D2D1::Point2U(0, 0);

	m_floor->BeginDraw();
	m_floor->DrawBitmap(m_background.Get(), viewportRect);
	DX::ThrowIfFailed(
		m_floor->EndDraw()
	);

	MFARGB colrect2;
	colrect2.rgbAlpha = 192;
	colrect2.rgbRed = 9;
	colrect2.rgbGreen = 2;
	colrect2.rgbBlue = 198;
	D2D1_RECT_F rect;
	Microsoft::WRL::ComPtr<ID2D1Brush> brrect2 = m_Brushes.WannaHave(m_d2dContext, colrect2);
	
	m_rooof->BeginDraw();
	rect.left = 50 + 25;
	rect.top = 2 * 49;
	rect.right = rect.left + 49;
	rect.bottom = rect.top + 49;
	m_rooof->FillRectangle(rect, brrect2.Get());
	DX::ThrowIfFailed(
		m_rooof->EndDraw()
	);
}

void WorldScreenBase::Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition)
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

	m_walls->GetBitmap(&bmp);
	m_d2dContext->DrawBitmap(bmp, screenRect);
	bmp->Release();

	m_rooof->GetBitmap(&bmp);
	m_d2dContext->DrawBitmap(bmp, screenRect);
	bmp->Release();

	WorldScreenBase::DrawLevelEditorRaster();

    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    m_d2dContext->RestoreDrawingState(m_stateBlock.Get());
}

void WorldScreenBase::DrawLevelEditorRaster()
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