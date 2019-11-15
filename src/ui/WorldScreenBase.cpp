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
	this->m_currentLevel = nullptr;	
	this->m_hoveringSheetNW = nullptr;
	this->m_hoveringSheetNE = nullptr;
	this->m_hoveringSheetSE = nullptr;
	this->m_hoveringSheetSW = nullptr;
}

WorldScreenBase::~WorldScreenBase()
{	
	while (!this->m_Sheets.empty())
	{
		delete this->m_Sheets.back();
		this->m_Sheets.pop_back();
	}

	delete this->m_currentLevel;
}

void WorldScreenBase::Initialize(_In_ std::shared_ptr<DX::DeviceResources>&	deviceResources)
{
	this->m_deviceResources = deviceResources;
	this->m_wicFactory = deviceResources->GetWicImagingFactory();
	this->m_d2dDevice = deviceResources->GetD2DDevice();
	this->m_d2dContext = deviceResources->GetD2DDeviceContext();
	this->m_backgroundSize = D2D1::SizeF(0.0f, 0.0f);
	this->m_viewportSize = D2D1::SizeF(0, 0);
	this->m_viewportSizeSquares = D2D1::SizeU(0.0f, 0.0f);

    ComPtr<ID2D1Factory> factory;
	this->m_d2dDevice->GetFactory(&factory);
    DX::ThrowIfFailed(factory.As(&this->m_d2dFactory));
	this->m_currentLevel->Initialize(this->m_d2dContext, &this->m_Brushes);
	this->CreateDeviceDependentResources();
	this->ResetDirectXResources();
}

void WorldScreenBase::CreateDeviceDependentResources()
{

}

void WorldScreenBase::ResetDirectXResources()
{
	auto loader = ref new BasicLoader(this->m_deviceResources->GetD3DDevice());
	loader->LoadPngToBitmap(L"Media\\Bitmaps\\notimeforcaution.png", m_deviceResources,	&this->m_notimeforcaution);
	loader->LoadPngToBitmap(L"Media\\Bitmaps\\universe_seamless.png", m_deviceResources, &this->m_background);
	this->m_backgroundSize = this->m_background->GetSize();

#ifdef _DEBUG
	this->m_debugBorderBrush = this->m_Brushes.WannaHave(this->m_d2dContext, { 0,0,255,255 });
#endif
	/* rest of initialization */
	D2D1_SIZE_F canvasSize = this->m_d2dContext->GetSize();
	this->m_viewportSize.width = canvasSize.width;
	this->m_viewportSize.height = canvasSize.height;
	this->ComputeWorldSize();
	this->ComputeWorldCenter();
	this->ComputeViewportOffset();

    DX::ThrowIfFailed(this->m_d2dFactory->CreateDrawingStateBlock(&this->m_stateBlock));
    this->UpdateForWindowSizeChange();
}

void WorldScreenBase::ComputeWorldSize()
{
	auto worldSizeSquares = this->m_currentLevel->GetRectBoundsUnits();
	m_worldSize = D2D1::SizeF(worldSizeSquares.width * blooDot::Consts::SQUARE_WIDTH, worldSizeSquares.height * blooDot::Consts::SQUARE_HEIGHT);
}

void WorldScreenBase::ComputeWorldCenter()
{
	auto worldSizeSquares = this->m_currentLevel->GetRectBoundsUnits();
	m_worldCenterSquares = D2D1::Point2U(worldSizeSquares.width / 2, worldSizeSquares.width / 2);
	m_worldCenter = D2D1::Point2F(m_worldSize.width / 2.0, m_worldSize.height / 2.0);
}

void WorldScreenBase::ComputeViewportOffset()
{
	m_viewportSizeSquares = D2D1::SizeU(
		ceil(m_viewportSize.width / static_cast<float>(blooDot::Consts::SQUARE_WIDTH)),
		ceil(m_viewportSize.height / static_cast<float>(blooDot::Consts::SQUARE_HEIGHT))
	);

	m_viewportOffset = D2D1::Point2F(
		m_worldCenter.x - m_viewportSize.width / 2.0F,
		m_worldCenter.y - m_viewportSize.height / 2.0F
	);

	m_viewportOffsetSquares = D2D1::Point2U(
		m_worldCenterSquares.x - m_viewportSizeSquares.width / 2,
		m_worldCenterSquares.y - m_viewportSizeSquares.height / 2
	);
}

void WorldScreenBase::ReleaseDeviceDependentResources()
{
	this->m_d2dDevice.Reset();
	this->m_d2dContext.Reset();
	this->m_background.Reset();
	this->m_d2dFactory.Reset();
	this->m_stateBlock.Reset();
	this->m_wicFactory.Reset();
	this->m_Brushes.Reset();
	this->m_debugBorderBrush.Reset();
}

void WorldScreenBase::UpdateForWindowSizeChange()
{
	m_isResizing = true;
    Windows::Foundation::Rect windowBounds = CoreWindow::GetForCurrentThread()->Bounds;
	D2D1_SIZE_F canvasSize = m_d2dContext->GetSize();

	m_isResizing = false;
}

void WorldScreenBase::PlacePrimitive(ID2D1Bitmap *dingSurface, Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> renderTarget, Dings* ding, Facings coalesce, int placementX, int placementY)
{
	auto dingOnSheet = ding->GetSheetPlacement(coalesce);
	D2D1_RECT_F dingRect = D2D1::RectF(dingOnSheet.x * 49.0f, dingOnSheet.y * 49.0f, dingOnSheet.x * 49.0f + 49.0f, dingOnSheet.y * 49.0f + 49.0f);
	D2D1_RECT_F placementRect = D2D1::RectF(placementX * 49.0f, placementY * 49.0f, placementX * 49.0f + 49.0f, placementY * 49.0f + 49.0f);
	renderTarget->DrawBitmap(dingSurface, placementRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, dingRect);
}

void WorldScreenBase::SetControl(bool left, bool right, bool up, bool down)
{
	m_isMoving = Facings::Shy;

	if (left)
	{
		m_isMoving |= Facings::West;
	}
	else if (right)
	{
		m_isMoving |= Facings::East;
	}
	
	if (down)
	{
		m_isMoving |= Facings::South;
	}
	else if (up)
	{
		m_isMoving |= Facings::North;
	}
}

void WorldScreenBase::Update(float timeTotal, float timeDelta)
{
	if (m_isResizing) 
	{
		return;
	}

	/* now so which quadrants from which sheets do we need to cover the screen with
	 * the bit of the world the viewport is hovering over? that is here the question. */
	if (!this->m_sheetHoveringSituationKnown)
	{
		this->EvaluateSheetHoveringSituation();
	}

	int deltaX = 0, deltaY = 0;
	if (m_isMoving & Facings::East)
	{
		deltaX = 2;
	}
	else if (m_isMoving & Facings::West)
	{
		deltaX = -2;
	}
	
	if (m_isMoving & Facings::South)
	{
		deltaY = 2;
	}
	else if (m_isMoving & Facings::North)
	{
		deltaY = -2;
	}

	if (deltaX != 0 || deltaY != 0)
	{
		this->m_viewportOffset = D2D1::Point2F(m_viewportOffset.x + static_cast<float>(deltaX), m_viewportOffset.y + static_cast<float>(deltaY));
		if (this->m_hoveringSheetNW != nullptr)
		{
			auto viewPort = D2D1::RectF(this->m_viewportOffset.x, this->m_viewportOffset.y, this->m_viewportOffset.x + this->m_viewportSize.width, this->m_viewportOffset.y + this->m_viewportSize.height);
			this->m_hoveringSheetNW->Translate(viewPort, 0, 0);
		}
	}
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
	
#ifdef _DEBUG
	auto blitSheetCount = 0;
#endif
	if (this->m_hoveringSheetNW != nullptr)
	{
		this->m_hoveringSheetNW->BlitToViewport();
#ifdef _DEBUG
		++blitSheetCount;
#endif
	}

	if (this->m_hoveringSheetNW != nullptr)
	{
		this->m_hoveringSheetNW->BlitToViewport();
#ifdef _DEBUG
		++blitSheetCount;
#endif
	}

	if (this->m_hoveringSheetNW != nullptr)
	{
		this->m_hoveringSheetNW->BlitToViewport();
#ifdef _DEBUG
		++blitSheetCount;
#endif
	}

	if (this->m_hoveringSheetNW != nullptr)
	{
		this->m_hoveringSheetNW->BlitToViewport();
#ifdef _DEBUG
		++blitSheetCount;
#endif
	}

    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    m_d2dContext->RestoreDrawingState(m_stateBlock.Get());

#ifdef _DEBUG
	const int bufferLength = 16;
	char16 str[bufferLength];
	int len = swprintf_s(str, bufferLength, L"%d", blitSheetCount);
	Platform::String^ string = ref new Platform::String(str, len);
	OutputDebugStringW(Platform::String::Concat(L"Number of sheets blitted: ", string)->Data());
#endif
}

void WorldScreenBase::EnterLevel(Level* level)
{
	this->m_currentLevel = level;

	/* generate the sheets for this level */
	auto subscriptX = level->GetNumOfSheetsWE();
	auto subscriptY = level->GetNumOfSheetsNS();
	for (auto y = 0; y < subscriptY; ++y)
	{
		for (auto x = 0; x < subscriptX; ++x)
		{
			this->m_Sheets.push_back((WorldSheet*)nullptr);
		}
	}
}

void WorldScreenBase::EvaluateSheetHoveringSituation()
{
	/* which sheets do we need populated to visualize,
	 * and which adjacents should we start populating?
	 * algorithm: we identify the sheet which has
	 * current viewport's center point in world coordinates,
	 * then we visit its neighbors and ask if they also intersect.
	 * given the minimal size per sheet is viewport size, this can
	 * never be more than 8 additional in close vicinity */
	auto centerIntersectsSheet = this->GetViewportCenterInLevel();
	auto sheetSize = this->m_currentLevel->GetSheetSizeUnits();
	auto intersectedSheetX = centerIntersectsSheet.x / sheetSize.width;
	auto intersectedSheetY = centerIntersectsSheet.y / sheetSize.height;
	auto sheetBound = m_currentLevel->GetNumOfSheetsWE();
	if (intersectedSheetX < 0)
	{
		intersectedSheetX = 0;
	}
	else if (intersectedSheetX >= sheetBound)
	{
		intersectedSheetX = sheetBound - 1;
	}

	sheetBound = this->m_currentLevel->GetNumOfSheetsNS();
	if (intersectedSheetY < 0)
	{
		intersectedSheetY = 0;
	}
	else if (intersectedSheetY >= sheetBound)
	{
		intersectedSheetY = sheetBound - 1;
	}

	auto centerSheet = this->GetSheet(intersectedSheetX, intersectedSheetY);
	if (!centerSheet->IsPopulated())
	{
		centerSheet->Populate();
		m_hoveringSheetNW = centerSheet;		
	}

	auto viewPort = D2D1::RectF(this->m_viewportOffset.x, this->m_viewportOffset.y, this->m_viewportOffset.x + this->m_viewportSize.width, this->m_viewportOffset.y + this->m_viewportSize.height);
	centerSheet->ComputeViewportOverlap(viewPort);

	this->m_sheetHoveringSituationKnown = true;
}

D2D1_POINT_2U WorldScreenBase::GetViewportCenterInLevel()
{
	return D2D1::Point2U(
		this->m_viewportOffsetSquares.x + this->m_viewportSizeSquares.width / 2,
		this->m_viewportOffsetSquares.y + this->m_viewportSizeSquares.height / 2
	);
}

void WorldScreenBase::InvalidateSheetHoveringSituation()
{
	this->m_sheetHoveringSituationKnown = false;
}

WorldSheet*	WorldScreenBase::GetSheet(unsigned sheetX, unsigned sheetY)
{
	auto sheetAddress = sheetY * m_currentLevel->GetNumOfSheetsWE() + sheetX;
	auto retrievedSheet = this->m_Sheets[sheetAddress];
	if (retrievedSheet == nullptr) 	
	{
		auto newSheet = new WorldSheet(m_deviceResources);
		newSheet->PrepareThyself(m_currentLevel, sheetX, sheetY);
		this->m_Sheets[sheetAddress] = newSheet;
		retrievedSheet = newSheet;
	}

	return retrievedSheet;
}