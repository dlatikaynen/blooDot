#include "..\PreCompiledHeaders.h"
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
	
	CreateDeviceDependentResources();
	ResetDirectXResources();
}

void LoadScreen::CreateDeviceDependentResources()
{

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
	m_Brushes.Reset();
	m_moved.width = m_moved.height = 0;

	delete m_GoL;
	delete m_GoL2;
	delete m_Sprinkler;
}

void LoadScreen::UpdateForWindowSizeChange()
{
	bool isAlive;

	m_isResizing = true;

    Windows::Foundation::Rect windowBounds = CoreWindow::GetForCurrentThread()->Bounds;

    m_offset.width = (windowBounds.Width - m_imageSize.width) / 2.0f;
    m_offset.height = (windowBounds.Height - m_imageSize.height) / 2.0f;

    m_totalSize.width = m_offset.width + m_imageSize.width;
    m_totalSize.height = m_offset.height + m_imageSize.height;

	/* we have the screen size here, so we use this opportunity to compute the GoL dimensions
	 * and also precompute the sprinkler circle dot matrix */
	D2D1_SIZE_F canvasSize = m_d2dContext->GetSize();
	int scaleFactor = (LoadScreen::GoLCellSideLength + 1) + 1;
	int gridWidth = static_cast<int>(canvasSize.width / scaleFactor);
	int gridHeight = static_cast<int>(canvasSize.height / scaleFactor);
	int pixWidth = LoadScreen::GoLCellSideLength;
	int pixHeight = LoadScreen::GoLCellSideLength;

	m_GoL = new GameOfLifePlane(gridWidth, gridHeight);

	srand(static_cast<unsigned int>(time(NULL)));
	for (int i = 0; i < m_GoL->GetWidth(); ++i)
	{
		for (int j = 0; j < m_GoL->GetHeight(); ++j)
		{
			isAlive = !((rand() % (rand() % 4 + 1)) == 0);
			m_GoL->SetAlive(i, j, isAlive);
		}
	}

	/* the swap plane */
	m_GoL2 = new GameOfLifePlane(m_GoL->GetWidth(), m_GoL->GetHeight());

	/* the sprinkler */
	m_Sprinkler = new GameOfLifeSprinkler();
	m_Sprinkler->Create(LoadScreen::GoLSprinklerRadius);

	m_isResizing = false;
}

void LoadScreen::Update(float timeTotal, float timeDelta)
{
	if (m_isResizing) 
	{
		return;
	}

	//m_moved.width++;

	m_GoLEngine.SetInitialMatrix(m_GoL);
	m_GoLEngine.SinlgeStep();
	m_GoL = m_GoLEngine.GetCurrentMatrix();

	/*
		Births: Each dead cell adjacent to exactly three live neighbors will become live in the next generation.
		Death by isolation: Each live cell with one or fewer live neighbors will die in the next generation.
		Death by overcrowding: Each live cell with four or more live neighbors will die in the next generation.
		Survival: Each live cell with either two or three live neighbors will remain alive for the next generation.
	*/
	//for (int i = 0; i < m_GoL->GetWidth(); ++i) for (int j = 0; j < m_GoL->GetHeight(); ++j)
	//{
	//	bool curState = m_GoL->CellAt(i, j)->IsAlive();
	//	int NAl = m_GoL->NeighborsAlive(i, j);
	//	if (curState)
	//	{
	//		if (NAl == 2 || NAl == 3)
	//		{
	//			/* survives */
	//			m_GoL2->SetAlive(i, j);
	//		}
	//		else if (NAl < 2)
	//		{
	//			/* starvation */
	//			m_GoL2->SetAlive(i, j, false);
	//		}
	//		else if (NAl > 3)
	//		{
	//			/* overcrowded */
	//			m_GoL2->SetAlive(i, j, false);
	//		}
	//	}
	//	else
	//	{
	//		/* birth out of thin air */
	//		if (NAl == 3)
	//		{
	//			m_GoL2->SetAlive(i, j);
	//		}
	//	}
	//}

	///* and then the rain set in */
	//int k = 0;
	//for (int z = 0; z < m_GoL->GetWidth(); ++z) 
	//{
	//	int i = rand() % m_GoL->GetWidth();
	//	int j = rand() % m_GoL->GetHeight();
	//	if (!m_GoL2->CellAt(i, j)->IsAlive())
	//	{
	//		m_GoL2->CellAt(i, j)->MakeRaindrop(true);
	//		++k;
	//	}
	//	if (k == 9) break;
	//}

	//for (int x = 0; x < m_GoL2->GetWidth(); ++x)
	//{
	//	for (int y = 0; y < m_GoL2->GetHeight(); ++y)
	//	{
	//		m_GoL->SetAlive(x, y, m_GoL2->CellAt(x, y)->IsAlive());
	//		m_GoL->SetRaindrop(x, y, m_GoL2->CellAt(x, y)->IsRaindrop());
	//	}
	//}
}

void LoadScreen::Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition)
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

	int planeWidth = m_GoL->GetWidth();
	int planeHeight = m_GoL->GetHeight();
	for (int i = 0; i < planeWidth; ++i) for (int j = 0; j < planeHeight; ++j)
	{
		GameOfLifeCell* cell = m_GoL->CellAt(i, j);
		if (cell->IsAlive())
		{
			cell->DrawCell(m_d2dContext, i, j, LoadScreen::GoLCellSideLength, &m_Brushes);
		}
	}

	D2D1_POINT_2L pos;
	pos.x = static_cast<int>(pointerPosition.x);
	pos.y = static_cast<int>(pointerPosition.y);

	//m_d2dContext->FillEllipse(
	//	D2D1::Ellipse(pos, 30, 30),
	//	m_GoLBrushRaindrop.Get()
	//);

	m_Sprinkler->Render(m_d2dContext, pos.x, pos.y, LoadScreen::GoLCellSideLength, &m_Brushes);

	//ComPtr<ID2D1Effect> scaleEffect;
	//m_d2dContext->CreateEffect(CLSID_D2D1Scale, &scaleEffect);
	//scaleEffect->SetInput(0, bitmap);
	//scaleEffect->SetValue(D2D1_SCALE_PROP_CENTER_POINT, D2D1::Vector2F(256.0f, 192.0f));
	//scaleEffect->SetValue(D2D1_SCALE_PROP_SCALE, D2D1::Vector2F(2.0f, 2.0f));
	//m_d2dContext->DrawImage(scaleEffect.Get());


    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    m_d2dContext->RestoreDrawingState(m_stateBlock.Get());
}
