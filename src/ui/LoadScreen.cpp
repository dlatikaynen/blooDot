#include "..\PreCompiledHeaders.h"
#include "LoadScreen.h"
#include "..\algo\BitmapPixelator.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;

LoadScreen::LoadScreen()
{
	this->m_Brushes = std::make_shared<BrushRegistry>();
	this->m_resourceLoadingCompleted = false;
}

void LoadScreen::Initialize(_In_ std::shared_ptr<DX::DeviceResources>& deviceResources, _In_ std::shared_ptr<Audio> audioEngine)
{
	this->m_deviceResources = deviceResources;
	this->m_wicFactory = deviceResources->GetWicImagingFactory();
	this->m_d2dDevice = deviceResources->GetD2DDevice();
	this->m_d2dContext = deviceResources->GetD2DDeviceContext();
	this->m_audio = audioEngine;
	this->m_imageSize = D2D1::SizeF(0.0f, 0.0f);
	this->m_offset = D2D1::SizeF(0.0f, 0.0f);
	this->m_moved = D2D1::SizeF(0.0f, 0.0f);
	this->m_totalSize = D2D1::SizeF(0.0f, 0.0f);

    ComPtr<ID2D1Factory> factory;
	this->m_d2dDevice->GetFactory(&factory);
    DX::ThrowIfFailed(
        factory.As(&this->m_d2dFactory)
    );

	this->m_Synthesizer = std::make_shared<SynthOfLife>();
	this->CreateDeviceDependentResources();
	this->ResetDirectXResources();
}

void LoadScreen::CreateDeviceDependentResources()
{

}

void LoadScreen::ResourceLoadingCompleted()
{
	this->m_resourceLoadingCompleted = true;
}

void LoadScreen::ResetDirectXResources()
{
    ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
    DX::ThrowIfFailed(
		this->m_wicFactory->CreateDecoderFromFilename(
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
        this->m_wicFactory->CreateFormatConverter(&wicFormatConverter)
    );

    DX::ThrowIfFailed(
        wicFormatConverter->Initialize(
            wicBitmapFrame.Get(),
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom // the BGRA format has no palette, so this value is ignored
        )
    );

    DX::ThrowIfFailed(
        this->m_d2dContext->CreateBitmapFromWicBitmap(
            wicFormatConverter.Get(),
            BitmapProperties(PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            &this->m_bitmap
        )
    );

    this->m_imageSize = this->m_bitmap->GetSize();
    DX::ThrowIfFailed(
        this->m_d2dFactory->CreateDrawingStateBlock(&this->m_stateBlock)
    );

    this->UpdateForWindowSizeChange();
}

void LoadScreen::ReleaseDeviceDependentResources()
{
    /*m_d2dDevice.Reset();
    m_d2dContext.Reset();
    m_bitmap.Reset();
    m_d2dFactory.Reset();
    m_stateBlock.Reset();
    m_wicFactory.Reset();
	m_Brushes.Reset();*/

	this->m_moved.width = this->m_moved.height = 0;
	delete this->m_Sprinkler;
}

void LoadScreen::UpdateForWindowSizeChange()
{
	bool isAlive;
	this->m_isResizing = true;
    Windows::Foundation::Rect windowBounds = CoreWindow::GetForCurrentThread()->Bounds;
	FLOAT dpiX, dpiY;
	this->m_d2dContext->GetDpi(&dpiX, &dpiY);
	Windows::Foundation::Rect viewportBounds = Windows::Foundation::Rect(
		Windows::Foundation::Point(
			(float)DipsToPixelsX(windowBounds.Left, dpiX),
			(float)DipsToPixelsY(windowBounds.Top, dpiY)
		),
		Windows::Foundation::Point(
			(float)DipsToPixelsX(windowBounds.Right, dpiX),
			(float)DipsToPixelsY(windowBounds.Bottom, dpiY)
		)
	);

	this->m_offset.width = (viewportBounds.Width - this->m_imageSize.width) / 2.0f;
	this->m_offset.height = (viewportBounds.Height - this->m_imageSize.height) / 2.0f;
	this->m_totalSize.width = this->m_offset.width + this->m_imageSize.width;
	this->m_totalSize.height = this->m_offset.height + this->m_imageSize.height;

	/* we have the screen size here, so we use this opportunity to compute the GoL dimensions
	 * and also precompute the sprinkler circle dot matrix */
	//D2D1_SIZE_F canvasSize = m_d2dContext->GetSize();
	int scaleFactor = (LoadScreen::GoLCellSideLength + 1) + 1;
	int gridWidth = static_cast<int>(viewportBounds.Width / scaleFactor);
	int gridHeight = static_cast<int>(viewportBounds.Height / scaleFactor);
	int pixWidth = LoadScreen::GoLCellSideLength;
	int pixHeight = LoadScreen::GoLCellSideLength;

	GameOfLifePlane* randGoL = new GameOfLifePlane(gridWidth, gridHeight);
	randGoL->Wipe();

	srand(static_cast<unsigned int>(time(NULL)));
	for (int i = 0; i < randGoL->GetWidth(); ++i)
	{
		for (int j = 0; j < randGoL->GetHeight(); ++j)
		{
			isAlive = !((rand() % (rand() % 4 + 1)) == 0);
			//randGoL->SetAlive(i, j, isAlive);
		}
	}

	/* the magic */
	BitmapPixelator bmPix;
	bmPix.Load(L"Media\\Bitmaps\\blooDot.bmp");
	bmPix.PlaceAt(randGoL, 10, 3);

	this->m_GoLEngine.SetInitialMatrix(randGoL);
	delete randGoL;

	this->m_GoLEngine.StartRecording();
	this->m_synthSequence = this->m_Synthesizer->Play(this->m_audio);

	/* the sprinkler */
	this->m_Sprinkler = new GameOfLifeSprinkler();
	this->m_Sprinkler->Create(LoadScreen::GoLSprinklerRadius);
	this->m_isResizing = false;
}

void LoadScreen::Update(float timeTotal, float timeDelta)
{
	if (this->m_isResizing)
	{
		return;
	}

	//m_moved.width++;
	this->m_GoLEngine.SingleStep();
	if (this->m_GoLEngine.IsRecording() && this->m_GoLEngine.GetNumStepsRecorded() == 100)
	{
		this->m_GoLEngine.EndRecording();
		this->m_GoLEngine.SaveRecording("GoLRecording1.bloodot");
	}

	//m_GoL = m_GoLEngine.GetCurrentMatrix();
	//* and then the rain set in */
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

	this->m_synthSequence->Update(timeTotal, timeDelta);
}

void LoadScreen::Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition)
{
	this->m_d2dContext->SaveDrawingState(this->m_stateBlock.Get());
	this->m_d2dContext->BeginDraw();
	this->m_d2dContext->SetTransform(orientation2D);

	//m_d2dContext->DrawBitmap(
 //       m_bitmap.Get(),
 //       D2D1::RectF(
 //           m_offset.width+m_moved.width,
 //           m_offset.height+m_moved.height,
 //           m_imageSize.width + m_offset.width+m_moved.width,
 //           m_imageSize.height + m_offset.height+m_moved.height
	//	)
 //   );

	auto plane = this->m_GoLEngine.GetCurrentMatrix();
	int planeWidth = plane->GetWidth();
	int planeHeight = plane->GetHeight();
	for (int i = 0; i < planeWidth; ++i) for (int j = 0; j < planeHeight; ++j)
	{
		auto cell = plane->CellAt(i, j);
		if (cell->IsAlive())
		{
			cell->DrawCell(this->m_d2dContext, i, j, LoadScreen::GoLCellSideLength, this->m_Brushes);
		}
	}

	D2D1_POINT_2L pos;
	pos.x = static_cast<int>(pointerPosition.x);
	pos.y = static_cast<int>(pointerPosition.y);

	//m_d2dContext->FillEllipse(
	//	D2D1::Ellipse(pos, 30, 30),
	//	m_GoLBrushRaindrop.Get()
	//);
	
	this->m_Sprinkler->Render(
		this->m_d2dContext,
		pos.x,
		pos.y,
		LoadScreen::GoLCellSideLength,
		this->m_Brushes
	);

	//ComPtr<ID2D1Effect> scaleEffect;
	//m_d2dContext->CreateEffect(CLSID_D2D1Scale, &scaleEffect);
	//scaleEffect->SetInput(0, bitmap);
	//scaleEffect->SetValue(D2D1_SCALE_PROP_CENTER_POINT, D2D1::Vector2F(256.0f, 192.0f));
	//scaleEffect->SetValue(D2D1_SCALE_PROP_SCALE, D2D1::Vector2F(2.0f, 2.0f));
	//m_d2dContext->DrawImage(scaleEffect.Get());

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = this->m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    this->m_d2dContext->RestoreDrawingState(this->m_stateBlock.Get());
	if (this->m_resourceLoadingCompleted)
	{
		this->m_synthSequence->Render();
	}
}
