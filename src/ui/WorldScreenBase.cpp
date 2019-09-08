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
	m_imageSize = D2D1::SizeF(0.0f, 0.0f);
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
            &m_bitmap
        )
    );

    m_imageSize = m_bitmap->GetSize();

    DX::ThrowIfFailed(
        m_d2dFactory->CreateDrawingStateBlock(&m_stateBlock)
    );

    UpdateForWindowSizeChange();
}

void WorldScreenBase::ReleaseDeviceDependentResources()
{
    m_d2dDevice.Reset();
    m_d2dContext.Reset();
    m_bitmap.Reset();
    m_d2dFactory.Reset();
    m_stateBlock.Reset();
    m_wicFactory.Reset();
	m_Brushes.Reset();
}

void WorldScreenBase::UpdateForWindowSizeChange()
{
	m_isResizing = true;

    Windows::Foundation::Rect windowBounds = CoreWindow::GetForCurrentThread()->Bounds;

    m_totalSize.width = m_imageSize.width;
    m_totalSize.height = m_imageSize.height;
	D2D1_SIZE_F canvasSize = m_d2dContext->GetSize();

	m_isResizing = false;
}

void WorldScreenBase::Update(float timeTotal, float timeDelta)
{
	if (m_isResizing) 
	{
		return;
	}
}

void WorldScreenBase::Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition)
{
    m_d2dContext->SaveDrawingState(m_stateBlock.Get());
    m_d2dContext->BeginDraw();
    m_d2dContext->SetTransform(orientation2D);

	m_d2dContext->DrawBitmap(
        m_bitmap.Get(),
        D2D1::RectF(
            0,
            0,
            m_imageSize.width,
            m_imageSize.height
		)
    );

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
	MFARGB colrect;
	colrect.rgbAlpha = 192;
	colrect.rgbRed = 196;
	colrect.rgbGreen = 3;
	colrect.rgbBlue = 7;
	MFARGB colrect2;
	colrect2.rgbAlpha = 192;
	colrect2.rgbRed = 9;
	colrect2.rgbGreen = 2;
	colrect2.rgbBlue = 198;
	D2D1_POINT_2F point0, point1;
	D2D1_RECT_F rect;
	Microsoft::WRL::ComPtr<ID2D1Brush> brush = m_Brushes.WannaHave(m_d2dContext, color);
	Microsoft::WRL::ComPtr<ID2D1Brush> highlight = m_Brushes.WannaHave(m_d2dContext, colhigh);
	Microsoft::WRL::ComPtr<ID2D1Brush> brrect = m_Brushes.WannaHave(m_d2dContext, colrect);
	Microsoft::WRL::ComPtr<ID2D1Brush> brrect2 = m_Brushes.WannaHave(m_d2dContext, colrect2);

	rect.left = 49;
	rect.top = 98;
	rect.right = rect.left + 49;
	rect.bottom = rect.top + 49;
	m_d2dContext->FillRectangle(rect, brrect.Get());

	rect.left = 98;
	rect.top = 98;
	rect.right = rect.left + 98;
	rect.bottom = rect.top + 49;
	m_d2dContext->FillRectangle(rect, brrect2.Get());

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