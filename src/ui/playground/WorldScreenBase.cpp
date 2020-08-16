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
	this->m_currentLevelEditorCell = D2D1::Point2U(0, 0);
	this->m_currentLevelEditorCellKnown = false;
	this->m_touchMap = nullptr;
#ifdef _DEBUG
	this->m_lastBlitSheetCount = 0;
#endif
}

WorldScreenBase::~WorldScreenBase()
{	
	WORLDSHEET_NW.reset();
	WORLDSHEET_NE.reset();
	WORLDSHEET_SE.reset();
	WORLDSHEET_SW.reset();
	//delete this->m_currentLevel;
}

void WorldScreenBase::Initialize(_In_ std::shared_ptr<Audio> audioEngine, _In_ std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	this->m_deviceResources = deviceResources;
	this->m_wicFactory = deviceResources->GetWicImagingFactory();
	this->m_d2dDevice = deviceResources->GetD2DDevice();
	this->m_d2dContext = deviceResources->GetD2DDeviceContext();	
	this->m_viewportSize = D2D1::SizeF(0.0f, 0.0f);
	this->m_viewportSizeSquares = D2D1::SizeU(0, 0);
	this->m_audio = audioEngine;
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
	this->m_projectileBrush = this->m_Brushes.WannaHave(this->m_d2dContext, { 0, 0, 250, 255 });

#ifdef _DEBUG
	this->m_debugBorderBrush = this->m_Brushes.WannaHave(this->m_d2dContext, { 0, 0, 255, 255 });
	this->m_debugTresholdBrush = this->m_Brushes.WannaHave(this->m_d2dContext, { 0, 240, 247, 255 });
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
	this->m_viewportSizeSquares = D2D1::SizeU(
		static_cast<unsigned>(ceil(this->m_viewportSize.width / static_cast<float>(blooDot::Consts::SQUARE_WIDTH))),
		static_cast<unsigned>(ceil(this->m_viewportSize.height / static_cast<float>(blooDot::Consts::SQUARE_HEIGHT)))
	);

	this->m_viewportOffset = D2D1::Point2F(
		this->m_worldCenter.x - m_viewportSize.width / 2.0F,
		this->m_worldCenter.y - m_viewportSize.height / 2.0F
	);

	this->m_viewportOffsetSquares = D2D1::Point2U(
		static_cast<unsigned>(this->m_viewportOffset.x / blooDot::Consts::SQUARE_WIDTH),
		static_cast<unsigned>(this->m_viewportOffset.y / blooDot::Consts::SQUARE_HEIGHT)
	);

	auto tresholdWidth = this->m_viewportSize.width * SCROLL_TRESHOLD_FACT / 2.0f;
	auto tresholdHeight = this->m_viewportSize.height * SCROLL_TRESHOLD_FACT / 2.0f;
	this->m_viewportScrollTreshold = D2D1::RectF(
		this->m_viewportSize.width / 2.0f - tresholdWidth,
		this->m_viewportSize.height / 2.0f - tresholdHeight,
		this->m_viewportSize.width / 2.0f + tresholdWidth,
		this->m_viewportSize.height / 2.0f + tresholdHeight
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
	this->m_isResizing = true;
    Windows::Foundation::Rect windowBounds = CoreWindow::GetForCurrentThread()->Bounds;
	D2D1_SIZE_F canvasSize = this->m_d2dContext->GetSize();
	this->m_isResizing = false;
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
		if (WORLDSHEET_NW != nullptr)
		{
			WORLDSHEET_NW->Translate(viewPort, 0, 0);
		}

		if (WORLDSHEET_NE != nullptr)
		{
			WORLDSHEET_NE->Translate(viewPort, 0, 0);
		}

		if (WORLDSHEET_SE != nullptr)
		{
			WORLDSHEET_SE->Translate(viewPort, 0, 0);
		}

		if (WORLDSHEET_SW != nullptr)
		{
			WORLDSHEET_SW->Translate(viewPort, 0, 0);
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
	/* (re-)generate the sheets for this level */
	this->InvalidateSheetHoveringSituation();
	if (WORLDSHEET_NW != nullptr) 
	{
		WORLDSHEET_NW.reset();
	}

	if (WORLDSHEET_NE != nullptr)
	{
		WORLDSHEET_NE.reset();
	}

	if (WORLDSHEET_SW != nullptr)
	{
		WORLDSHEET_SW.reset();
	}

	if (WORLDSHEET_SE != nullptr)
	{
		WORLDSHEET_SE.reset();
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
	 * never be more than 8 additional in close vicinity,
	 * practically limited to the edge case of four */
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
	if (cellInSheetX > sheetSize.width / 2)
	{
		if (cellInSheetY > sheetSize.height / 2)
		{
			/* we are hit in our right, bottom, thus we are NW */
			WORLDSHEET_NW = this->InstantiateViewportSheet(intersectedSheetX, intersectedSheetY);
			WORLDSHEET_NE = this->InstantiateViewportSheet(intersectedSheetX + 1, intersectedSheetY);
			WORLDSHEET_SW = this->InstantiateViewportSheet(intersectedSheetX, intersectedSheetY + 1);
			WORLDSHEET_SE = this->InstantiateViewportSheet(intersectedSheetX + 1, intersectedSheetY + 1);
		}
		else
		{
			/* we are hit in out right, top, thus we are SW */
			WORLDSHEET_SW = this->InstantiateViewportSheet(intersectedSheetX, intersectedSheetY);
			WORLDSHEET_NW = this->InstantiateViewportSheet(intersectedSheetX, intersectedSheetY -1 );
			WORLDSHEET_NE = this->InstantiateViewportSheet(intersectedSheetX + 1, intersectedSheetY - 1);
			WORLDSHEET_SE = this->InstantiateViewportSheet(intersectedSheetX + 1, intersectedSheetY);
		}
	}
	else
	{
		if (cellInSheetY > sheetSize.height / 2)
		{
			/* we are hit in our left, bottom, so we are NE */
			WORLDSHEET_NE = this->InstantiateViewportSheet(intersectedSheetX, intersectedSheetY);
			WORLDSHEET_NW = this->InstantiateViewportSheet(intersectedSheetX - 1, intersectedSheetY);
			WORLDSHEET_SE = this->InstantiateViewportSheet(intersectedSheetX, intersectedSheetY + 1);
			WORLDSHEET_SW = this->InstantiateViewportSheet(intersectedSheetX - 1, intersectedSheetY + 1);
		}
		else
		{
			/* we are hit in our left, top, so we are SE */
			WORLDSHEET_SE = this->InstantiateViewportSheet(intersectedSheetX, intersectedSheetY);
			WORLDSHEET_SW = this->InstantiateViewportSheet(intersectedSheetX - 1, intersectedSheetY);
			WORLDSHEET_NW = this->InstantiateViewportSheet(intersectedSheetX - 1, intersectedSheetY - 1);
			WORLDSHEET_NE = this->InstantiateViewportSheet(intersectedSheetX, intersectedSheetY - 1);
		}
	}

	/* make sure the sheets have consciousness about their own whereabouts */
	WORLDSHEET_NW->Populate();
	WORLDSHEET_NE->Populate();
	WORLDSHEET_SW->Populate();
	WORLDSHEET_SE->Populate();
	
	/* finally, we have the quadruplet arranged,
	 * now prime the initial overlap rectangles */
	auto viewPort = D2D1::RectF(this->m_viewportOffset.x, this->m_viewportOffset.y, this->m_viewportOffset.x + this->m_viewportSize.width, this->m_viewportOffset.y + this->m_viewportSize.height);
	WORLDSHEET_NW->ComputeViewportOverlap(viewPort);
	WORLDSHEET_NE->ComputeViewportOverlap(viewPort);
	WORLDSHEET_SW->ComputeViewportOverlap(viewPort);
	WORLDSHEET_SE->ComputeViewportOverlap(viewPort);
	this->m_sheetHoveringSituationKnown = true;
}

void WorldScreenBase::ReflapBlitterSheets(D2D1_RECT_F viewPort, Facings towardsDirection)
{
	WorldSheet* sheetSwap;
	switch (towardsDirection)
	{
		case Facings::West:

			break;

		case Facings::East:
			sheetSwap = WORLDSHEET_NE.release();
			WORLDSHEET_NE = std::unique_ptr<WorldSheet>(WORLDSHEET_NW.release());
			WORLDSHEET_NW = std::unique_ptr<WorldSheet>(sheetSwap);
			WORLDSHEET_NE->Translate(viewPort, -WORLDSHEET_NW->PhysicalWidth(), 0);
			break;

		case Facings::North:
			
			break;

		case Facings::South:
		
			break;
	}
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

std::unique_ptr<WorldSheet>	WorldScreenBase::InstantiateViewportSheet(unsigned sheetX, unsigned sheetY)
{
	auto newSheet = std::make_unique<WorldSheet>(m_deviceResources);
	newSheet->PrepareThyself(m_currentLevel, sheetX, sheetY);
	return newSheet;
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

void WorldScreenBase::ObliterateObject(D2D1_POINT_2U levelCoordinate)
{
	Layers layerToCull;
	auto dingWeeded = this->m_currentLevel->WeedObjectAt(levelCoordinate.x, levelCoordinate.y, &layerToCull);
	if (dingWeeded != nullptr)
	{
		this->RedrawSingleSquare(levelCoordinate.x, levelCoordinate.y, layerToCull);
		if (dingWeeded->CouldCoalesce())
		{
			auto weededDingID = dingWeeded->ID();
			auto neighborHood = this->m_currentLevel->GetNeighborConfigurationOf(levelCoordinate.x, levelCoordinate.y, weededDingID, layerToCull);
			this->ClumsyPackNeighborhoodOf(neighborHood, levelCoordinate.x, levelCoordinate.y, layerToCull, weededDingID);
		}
	}
}

void WorldScreenBase::ClumsyPackNeighborhoodOf(ClumsyPacking::NeighborConfiguration neighborHood, unsigned aroundLevelX, unsigned aroundLevelY, Layers inLayer, Dings::DingIDs dingID)
{
	if ((neighborHood & 1) == 1)
	{
		this->ClumsyPackNeighborhoodOf(aroundLevelX - 1, aroundLevelY - 1, inLayer, dingID);
	}

	if ((neighborHood & 2) == 2)
	{
		this->ClumsyPackNeighborhoodOf(aroundLevelX, aroundLevelY - 1, inLayer, dingID);
	}

	if ((neighborHood & 4) == 4)
	{
		this->ClumsyPackNeighborhoodOf(aroundLevelX + 1, aroundLevelY - 1, inLayer, dingID);
	}

	if ((neighborHood & 8) == 8)
	{
		this->ClumsyPackNeighborhoodOf(aroundLevelX + 1, aroundLevelY, inLayer, dingID);
	}

	if ((neighborHood & 16) == 16)
	{
		this->ClumsyPackNeighborhoodOf(aroundLevelX + 1, aroundLevelY + 1, inLayer, dingID);
	}

	if ((neighborHood & 32) == 32)
	{
		this->ClumsyPackNeighborhoodOf(aroundLevelX, aroundLevelY + 1, inLayer, dingID);
	}

	if ((neighborHood & 64) == 64)
	{
		this->ClumsyPackNeighborhoodOf(aroundLevelX - 1, aroundLevelY + 1, inLayer, dingID);
	}

	if ((neighborHood & 128) == 128)
	{
		this->ClumsyPackNeighborhoodOf(aroundLevelX - 1, aroundLevelY, inLayer, dingID);
	}
}

void WorldScreenBase::ClumsyPackNeighborhoodOf(unsigned aroundLevelX, unsigned aroundLevelY, Layers inLayer, Dings::DingIDs dingID)
{
	auto centerObject = this->m_currentLevel->GetObjectAt(aroundLevelX, aroundLevelY, false);
	if (centerObject != nullptr)
	{
		auto centerDings = centerObject->GetDing(inLayer);
		if (centerDings != nullptr && centerDings->ID() == dingID)
		{
			auto neighborHood = this->m_currentLevel->GetNeighborConfigurationOf(aroundLevelX, aroundLevelY, dingID, inLayer);
			auto shouldBeFacing = ClumsyPacking::FacingFromConfiguration(neighborHood);
			if (centerObject->PlacementFacing(inLayer) != shouldBeFacing)
			{
				centerObject->AdjustFacing(inLayer, shouldBeFacing);
				this->RedrawSingleSquare(aroundLevelX, aroundLevelY, inLayer);
				this->ClumsyPackNeighborhoodOf(neighborHood, aroundLevelX, aroundLevelY, inLayer, dingID);
			}
		}
	}
}

void WorldScreenBase::RedrawSingleSquare(unsigned levelX, unsigned levelY, Layers inLayer)
{
	auto sheetSize = this->m_currentLevel->GetSheetSizeUnits();
	auto intersectedSheetX = levelX / sheetSize.width;
	auto cellInSheetX = levelX % sheetSize.width;
	auto intersectedSheetY = levelY / sheetSize.height;
	auto cellInSheetY = levelY % sheetSize.height;
	auto intersectNW = WORLDSHEET_NW->TheSheetIAm();
	if (intersectNW.x == intersectedSheetX && intersectNW.y == intersectedSheetY)
	{
		WORLDSHEET_NW->RedrawSingleSquare(cellInSheetX, cellInSheetY, inLayer);
	}
	else
	{
		auto intersectNE = WORLDSHEET_NE->TheSheetIAm();
		if (intersectNE.x == intersectedSheetX && intersectNE.y == intersectedSheetY)
		{
			WORLDSHEET_NE->RedrawSingleSquare(cellInSheetX, cellInSheetY, inLayer);
		}
		else
		{
			auto intersectSE = WORLDSHEET_SE->TheSheetIAm();
			if (intersectSE.x == intersectedSheetX && intersectSE.y == intersectedSheetY)
			{
				WORLDSHEET_SE->RedrawSingleSquare(cellInSheetX, cellInSheetY, inLayer);
			}
			else
			{
				auto intersectSW = WORLDSHEET_SW->TheSheetIAm();
				if (intersectSW.x == intersectedSheetX && intersectSW.y == intersectedSheetY)
				{
					WORLDSHEET_SW->RedrawSingleSquare(cellInSheetX, cellInSheetY, inLayer);
				}
			}
		}
	}
}