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
	ComPtr<IWICBitmapFrameDecode> wicBitmapFrame;
	ComPtr<IWICFormatConverter> wicFormatConverter;

	DX::ThrowIfFailed(
        m_wicFactory->CreateDecoderFromFilename(
            L"Media\\Bitmaps\\universe_seamless.png",
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnDemand,
            &wicBitmapDecoder
        )
    );

    DX::ThrowIfFailed(wicBitmapDecoder->GetFrame(0, &wicBitmapFrame));
    DX::ThrowIfFailed(m_wicFactory->CreateFormatConverter(&wicFormatConverter));
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

	/* second bitmap loading */
	DX::ThrowIfFailed(
		m_wicFactory->CreateDecoderFromFilename(
			L"Media\\Bitmaps\\notimeforcaution.png",
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&wicBitmapDecoder
		)
	);

	DX::ThrowIfFailed(wicBitmapDecoder->GetFrame(0, &wicBitmapFrame));
	DX::ThrowIfFailed(m_wicFactory->CreateFormatConverter(&wicFormatConverter));
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
			&m_notimeforcaution
		)
	);
	
	/* rest of initialization */
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
	auto dalek = Dalek(m_Brushes);

	m_dings->BeginDraw();

	deflt.Draw(m_dings, 0, 0);
	mauer.Draw(m_dings, 1, 1);
	dalek.Draw(m_dings, 1, 0);

	DX::ThrowIfFailed(
		m_dings->EndDraw()
	);

	ID2D1Bitmap *dings = NULL;
	m_dings->GetBitmap(&dings);

	m_walls->BeginDraw();
	PlacePrimitive(dings, m_walls, &mauer, Facings::Shy, 0, 0);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Shy, 1, 0);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Shy, 2, 0);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Shy, 3, 1);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Shy, 3, 2);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Shy, 4, 2);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Immersed, 4, 3);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Center, 5, 3);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Shy, 6, 3);

	PlacePrimitive(dings, m_walls, &mauer, Facings::North, 3, 4);
	PlacePrimitive(dings, m_walls, &mauer, Facings::South, 3, 5);
	PlacePrimitive(dings, m_walls, &mauer, Facings::West, 2, 6);
	PlacePrimitive(dings, m_walls, &mauer, Facings::East, 3, 6);

	PlacePrimitive(dings, m_walls, &mauer, Facings::NS, 5, 2);
	PlacePrimitive(dings, m_walls, &mauer, Facings::WE, 6, 3);
	PlacePrimitive(dings, m_walls, &mauer, Facings::WE, 7, 3);
	PlacePrimitive(dings, m_walls, &mauer, Facings::WE, 8, 3);
	PlacePrimitive(dings, m_walls, &mauer, Facings::WE, 9, 3);
	PlacePrimitive(dings, m_walls, &mauer, Facings::WE, 10, 3);
	PlacePrimitive(dings, m_walls, &mauer, Facings::WE, 11, 3);
	PlacePrimitive(dings, m_walls, &mauer, Facings::WE, 12, 3);
	PlacePrimitive(dings, m_walls, &mauer, Facings::East, 13, 3);

	PlacePrimitive(dings, m_walls, &mauer, Facings::CornerNWSE, 8, 5);
	PlacePrimitive(dings, m_walls, &mauer, Facings::CornerSWNE, 9, 5);

	PlacePrimitive(dings, m_walls, &mauer, Facings::TN, 6, 0);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Center, 6, 1);
	PlacePrimitive(dings, m_walls, &mauer, Facings::TW, 5, 1);
	PlacePrimitive(dings, m_walls, &mauer, Facings::TE, 7, 1);
	PlacePrimitive(dings, m_walls, &mauer, Facings::TS, 6, 2);

	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeN, 9, 0);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeNW, 8, 0);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeE, 10, 1);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeNE, 10, 0);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeW, 8, 1);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeSW, 8, 2);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeS, 9, 2);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeSE, 10, 2);

	PlacePrimitive(dings, m_walls, &mauer, Facings::NW, 12, 0);
	PlacePrimitive(dings, m_walls, &mauer, Facings::NE, 13, 0);
	PlacePrimitive(dings, m_walls, &mauer, Facings::SW, 12, 1);
	PlacePrimitive(dings, m_walls, &mauer, Facings::SE, 13, 1);

	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeCornerED, 15, 0);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeCornerEU, 16, 0);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeCornerNL, 15, 1);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeCornerNR, 16, 1);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeCornerWU, 15, 2);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeCornerWD, 16, 2);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeCornerSL, 15, 3);
	PlacePrimitive(dings, m_walls, &mauer, Facings::EdgeCornerSL, 16, 3);

	PlacePrimitive(dings, m_walls, &mauer, Facings::Corner3SE, 18, 0);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Corner3SW, 19, 0);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Corner3NE, 18, 1);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Corner3NW, 19, 1);

	PlacePrimitive(dings, m_walls, &mauer, Facings::Corner2N, 18, 3);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Corner2W, 19, 3);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Corner2E, 18, 4);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Corner2S, 19, 4);

	PlacePrimitive(dings, m_walls, &mauer, Facings::Corner1SE, 18, 6);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Corner1SW, 19, 6);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Corner1NE, 18, 7);
	PlacePrimitive(dings, m_walls, &mauer, Facings::Corner1NW, 19, 7);

	PlacePrimitive(dings, m_walls, &dalek, Facings::Shy, 1, 4);

	/* a preview on decor */
	m_walls->DrawBitmap(m_notimeforcaution.Get(), D2D1::RectF(6.5f * 49.0f, 3.26f * 49.0f, 6.5f * 49.0f + 301.0f, 3.26f * 49.0f + 22.0f));
	
	dings->Release();
	DX::ThrowIfFailed(
		m_walls->EndDraw()
	);

	m_isResizing = false;
}

void WorldScreenBase::PlacePrimitive(ID2D1Bitmap *dingSurface, Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> renderTarget, Dings* ding, Facings coalesce, int placementX, int placementY)
{
	auto dingOnSheet = ding->GetSheetPlacement(coalesce);
	D2D1_RECT_F dingRect = D2D1::RectF(dingOnSheet.x * 49.0f, dingOnSheet.y * 49.0f, dingOnSheet.x * 49.0f + 49.0f, dingOnSheet.y * 49.0f + 49.0f);
	D2D1_RECT_F placementRect = D2D1::RectF(placementX * 49.0f, placementY * 49.0f, placementX * 49.0f + 49.0f, placementY * 49.0f + 49.0f);
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

    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    m_d2dContext->RestoreDrawingState(m_stateBlock.Get());
}