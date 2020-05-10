#include "..\..\PreCompiledHeaders.h"
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
	this->m_currentLevelEditorCell = D2D1::Point2U(0, 0);
	this->m_currentLevelEditorCellKnown = false;
	this->m_touchMap = nullptr;
#ifdef _DEBUG
	this->m_lastBlitSheetCount = 0;
#endif
}

WorldScreenBase::~WorldScreenBase()
{	
	while (!this->m_Sheets.empty())
	{
		delete this->m_Sheets.back();
		this->m_Sheets.pop_back();
	}

	//delete this->m_currentLevel;
}

void WorldScreenBase::Initialize(_In_ std::shared_ptr<DX::DeviceResources>&	deviceResources)
{
	this->m_deviceResources = deviceResources;
	this->m_wicFactory = deviceResources->GetWicImagingFactory();
	this->m_d2dDevice = deviceResources->GetD2DDevice();
	this->m_d2dContext = deviceResources->GetD2DDeviceContext();	
	this->m_viewportSize = D2D1::SizeF(0.0f, 0.0f);
	this->m_viewportSizeSquares = D2D1::SizeU(0, 0);

    ComPtr<ID2D1Factory> factory;
	this->m_d2dDevice->GetFactory(&factory);
    DX::ThrowIfFailed(factory.As(&this->m_d2dFactory));
	this->m_currentLevel->Initialize(this->m_deviceResources, &this->m_Brushes);
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
	m_worldCenter = D2D1::Point2F(m_worldSize.width / 2.0f, m_worldSize.height / 2.0f);
}

void WorldScreenBase::ComputeViewportOffset()
{
	m_viewportSizeSquares = D2D1::SizeU(
		static_cast<unsigned>(ceil(m_viewportSize.width / static_cast<float>(blooDot::Consts::SQUARE_WIDTH))),
		static_cast<unsigned>(ceil(m_viewportSize.height / static_cast<float>(blooDot::Consts::SQUARE_HEIGHT)))
	);

	m_viewportOffset = D2D1::Point2F(
		m_worldCenter.x - m_viewportSize.width / 2.0F,
		m_worldCenter.y - m_viewportSize.height / 2.0F
	);

	this->m_viewportOffsetSquares = D2D1::Point2U(
		static_cast<unsigned>(this->m_viewportOffset.x / blooDot::Consts::SQUARE_WIDTH),
		static_cast<unsigned>(this->m_viewportOffset.y / blooDot::Consts::SQUARE_HEIGHT)
	);
}

void WorldScreenBase::ReleaseDeviceDependentResources()
{
//	this->m_d2dDevice.Reset();
//	this->m_d2dContext.Reset();	
//	this->m_d2dFactory.Reset();
//	this->m_stateBlock.Reset();
//	this->m_wicFactory.Reset();
//	this->m_Brushes.Reset();
//#ifdef _DEBUG
//	this->m_debugBorderBrush.Reset();
//#endif
}

void WorldScreenBase::UpdateForWindowSizeChange()
{
	m_isResizing = true;
    Windows::Foundation::Rect windowBounds = CoreWindow::GetForCurrentThread()->Bounds;
	D2D1_SIZE_F canvasSize = m_d2dContext->GetSize();

	m_isResizing = false;
}

void WorldScreenBase::SetControl(
	DirectX::XMFLOAT2 pointerPosition, 
	TouchMap* touchMap, 
	bool shiftKeyActive, 
	bool left, 
	bool right, 
	bool up, 
	bool down, 
	float scrollDeltaX, 
	float scrollDeltaY
)
{
	this->m_pointerPosition.x = pointerPosition.x;
	this->m_pointerPosition.y = pointerPosition.y;
	this->m_touchMap = touchMap;
	this->m_isMoving = Facings::Shy;
	this->m_keyShiftDown = shiftKeyActive;
	if (abs(scrollDeltaX) > 0.15f || abs(scrollDeltaY) > 0.15f)
	{		
		if (scrollDeltaX > 0)
		{
			this->m_isMoving |= Facings::East;
			this->m_movingSpeedX = static_cast<unsigned>(scrollDeltaX * 15.0f);
		}
		else if (scrollDeltaX < 0)
		{
			this->m_isMoving |= Facings::West;
			this->m_movingSpeedX = static_cast<unsigned>(-scrollDeltaX * 15.0f);
		}

		if (scrollDeltaY > 0)
		{
			this->m_isMoving |= Facings::North;
			this->m_movingSpeedY = static_cast<unsigned>(scrollDeltaY * 15.0f);
		}
		else if (scrollDeltaY < 0)
		{
			this->m_isMoving |= Facings::South;
			this->m_movingSpeedY = static_cast<unsigned>(-scrollDeltaY * 15.0f);
		}
	}
	else
	{
		if (left)
		{
			this->m_isMoving |= Facings::West;
		}
		else if (right)
		{
			this->m_isMoving |= Facings::East;
		}

		if (down)
		{
			this->m_isMoving |= Facings::South;
		}
		else if (up)
		{
			this->m_isMoving |= Facings::North;
		}

		if (this->m_isMoving != Facings::Shy)
		{
			this->m_movingSpeedX = shiftKeyActive ? 4 : 2;
			this->m_movingSpeedY = shiftKeyActive ? 4 : 2;
		}
	}
}

// mousewheeled
void WorldScreenBase::SetControl(int detentCount, bool shiftKeyActive)
{

}

void WorldScreenBase::Update(float timeTotal, float timeDelta)
{
	if (this->m_isResizing) 
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
	if (this->m_isMoving & Facings::East)
	{
		deltaX = this->m_movingSpeedX;
	}
	else if (this->m_isMoving & Facings::West)
	{
		deltaX = -static_cast<int>(this->m_movingSpeedX);
	}
	
	if (this->m_isMoving & Facings::South)
	{
		deltaY = this->m_movingSpeedY;
	}
	else if (this->m_isMoving & Facings::North)
	{
		deltaY = -static_cast<int>(this->m_movingSpeedY);
	}

	if (deltaX != 0 || deltaY != 0)
	{
		this->m_viewportOffset = D2D1::Point2F(this->m_viewportOffset.x + static_cast<float>(deltaX), this->m_viewportOffset.y + static_cast<float>(deltaY));
		this->m_viewportOffsetSquares = D2D1::Point2U(
			static_cast<unsigned>(this->m_viewportOffset.x / blooDot::Consts::SQUARE_WIDTH),
			static_cast<unsigned>(this->m_viewportOffset.y / blooDot::Consts::SQUARE_HEIGHT)
		);

		auto viewPort = D2D1::RectF(this->m_viewportOffset.x, this->m_viewportOffset.y, this->m_viewportOffset.x + this->m_viewportSize.width, this->m_viewportOffset.y + this->m_viewportSize.height);
		if (this->m_hoveringSheetNW != nullptr)
		{
			this->m_hoveringSheetNW->Translate(viewPort, 0, 0);
		}

		if (this->m_hoveringSheetNE != nullptr)
		{
			this->m_hoveringSheetNE->Translate(viewPort, 0, 0);
		}

		if (this->m_hoveringSheetSE != nullptr)
		{
			this->m_hoveringSheetSE->Translate(viewPort, 0, 0);
		}

		if (this->m_hoveringSheetSW != nullptr)
		{
			this->m_hoveringSheetSW->Translate(viewPort, 0, 0);
		}
	}
}

std::shared_ptr<Level> WorldScreenBase::LoadLevel(Platform::String^ loadFromFile)
{
	auto levelName = ref new Platform::String(L"Gartenwelt-1");
	auto loadedLevel = std::make_shared<Level>(levelName, D2D1::SizeU(50, 30), 720, 720);
	loadedLevel->Initialize(this->m_deviceResources, &this->m_Brushes);
	loadedLevel->DesignLoadFromFile(loadFromFile);
	return loadedLevel;
}

std::shared_ptr<Level> WorldScreenBase::LoadAndEnterLevel(Platform::String^ loadFromFile)
{
	auto loadedLevel = this->LoadLevel(loadFromFile);
	this->EnterLevel(loadedLevel);
	return loadedLevel;
}

void WorldScreenBase::EnterLevel(std::shared_ptr<Level> level)
{
	this->m_currentLevel = level;
	this->InvalidateLevelViewport();
}

void WorldScreenBase::InvalidateLevelViewport()
{
	this->InvalidateSheetHoveringSituation();

	/* (re-)generate the sheets for this level */
	if (!this->m_Sheets.empty())
	{
		this->m_Sheets.clear();
	}

	auto subscriptX = this->m_currentLevel->GetNumOfSheetsWE();
	auto subscriptY = this->m_currentLevel->GetNumOfSheetsNS();
	for (unsigned y = 0; y < subscriptY; ++y)
	{
		for (unsigned x = 0; x < subscriptX; ++x)
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
	auto cellInSheetX = centerIntersectsSheet.x % sheetSize.width;
	auto intersectedSheetY = centerIntersectsSheet.y / sheetSize.height;
	auto cellInSheetY = centerIntersectsSheet.y % sheetSize.height;
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

	/* depending on where the center is inside the centersheet, we shall
	 * know where we are and which are our neighbor sheets */
	auto centerSheet = this->GetSheet(intersectedSheetX, intersectedSheetY);	
	if (cellInSheetX > sheetSize.width / 2)
	{
		if (cellInSheetY > sheetSize.height / 2)
		{
			/* we are hit in our right, bottom, thus we are NW */
			this->m_hoveringSheetNW = centerSheet;
			this->m_hoveringSheetNE = this->GetSheet(intersectedSheetX + 1, intersectedSheetY);
			this->m_hoveringSheetSW = this->GetSheet(intersectedSheetX, intersectedSheetY + 1);
			this->m_hoveringSheetSE = this->GetSheet(intersectedSheetX + 1, intersectedSheetY + 1);
		}
		else
		{
			/* we are hit in out right, top, thus we are SW */
			this->m_hoveringSheetSW = centerSheet;
			this->m_hoveringSheetNW = this->GetSheet(intersectedSheetX, intersectedSheetY -1 );
			this->m_hoveringSheetNE = this->GetSheet(intersectedSheetX + 1, intersectedSheetY - 1);
			this->m_hoveringSheetSE = this->GetSheet(intersectedSheetX + 1, intersectedSheetY);
		}
	}
	else
	{
		if (cellInSheetY > sheetSize.height / 2)
		{
			/* we are hit in our left, bottom, so we are NE */
			this->m_hoveringSheetNE = centerSheet;
			this->m_hoveringSheetNW = this->GetSheet(intersectedSheetX - 1, intersectedSheetY);
			this->m_hoveringSheetSE = this->GetSheet(intersectedSheetX, intersectedSheetY + 1);
			this->m_hoveringSheetSW = this->GetSheet(intersectedSheetX - 1, intersectedSheetY + 1);
		}
		else
		{
			/* we are hit in our left, top, so we are SE */
			this->m_hoveringSheetSE = centerSheet;
			this->m_hoveringSheetSW = this->GetSheet(intersectedSheetX - 1, intersectedSheetY);
			this->m_hoveringSheetNW = this->GetSheet(intersectedSheetX - 1, intersectedSheetY - 1);
			this->m_hoveringSheetNE = this->GetSheet(intersectedSheetX, intersectedSheetY - 1);
		}
	}

	/* make sure the sheets have consciousness about their own whereabouts */
	m_hoveringSheetNW->Populate();
	m_hoveringSheetNE->Populate();
	m_hoveringSheetSW->Populate();
	m_hoveringSheetSE->Populate();
	
	/* finally, we have the quadruplet arranged,
	 * now prime the initial overlap rectangles */
	auto viewPort = D2D1::RectF(this->m_viewportOffset.x, this->m_viewportOffset.y, this->m_viewportOffset.x + this->m_viewportSize.width, this->m_viewportOffset.y + this->m_viewportSize.height);
	m_hoveringSheetNW->ComputeViewportOverlap(viewPort);
	m_hoveringSheetNE->ComputeViewportOverlap(viewPort);
	m_hoveringSheetSW->ComputeViewportOverlap(viewPort);
	m_hoveringSheetSE->ComputeViewportOverlap(viewPort);
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
	if (this->m_sheetHoveringSituationKnown)
	{
		this->m_sheetHoveringSituationKnown = false;
	}
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

bool WorldScreenBase::PeekTouchdown()
{
	if (this->m_touchMap == nullptr)
	{
		return false;
	}
	else
	{
		return this->m_touchMap->size() > 0;
	}
}

bool WorldScreenBase::PopTouchdown()
{
	if (m_touchMap != nullptr && !m_touchMap->empty())
	{
		m_touchMap->erase(std::prev(m_touchMap->end()));
		return true;
	}

	return false;
}