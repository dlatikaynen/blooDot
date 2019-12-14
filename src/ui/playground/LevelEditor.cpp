#include "..\..\PreCompiledHeaders.h"
#include "..\..\dx\DirectXHelper.h"
#include "..\UserInterface.h"

LevelEditor::LevelEditor() : WorldScreenBase() 
{
	this->m_selectedDingID = 1;
	this->m_selectedDingOrientation = Facings::Shy;
	this->m_isGridShown = true;
	this->m_lastPlacementPositionValid = false;
	this->m_lastActiveGridPositionValid = false;
	this->m_keyShiftDown = false;
}

LevelEditor::~LevelEditor()
{	
	this->m_textColorBrush.Reset();
}

void LevelEditor::Initialize(_In_ std::shared_ptr<DX::DeviceResources>&	deviceResources)
{
	WorldScreenBase::Initialize(deviceResources);
	DX::ThrowIfFailed(deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &this->m_textColorBrush));
	this->m_textColorBrush->SetOpacity(m_textColorBrush->GetOpacity() * blooDot::Consts::GOLDEN_RATIO);
	this->m_textStyle.SetFontName(L"Segoe UI");
	this->m_textStyle.SetFontSize(11.0f);
	this->m_textStyle.SetFontWeight(DWRITE_FONT_WEIGHT_LIGHT);
	this->m_textStyle.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
}

void LevelEditor::Update(float timeTotal, float timeDelta)
{
	WorldScreenBase::Update(timeTotal, timeDelta);

	/* we would have a HUD showing the currently selected toolbox ding */
	auto myHUD = static_cast<LevelEditorHUD*>(UserInterface::GetInstance().GetElement(blooDot::UIElement::LevelEditorHUD));
	auto newOrientation = myHUD->SelectedDingOrientation();
	auto curDingID = myHUD->SelectedDingID();
	this->m_isGridShown = myHUD->IsGridShown();
	if (this->m_selectedDingID > 0 && (this->m_selectedDingID != curDingID || this->m_selectedDingOrientation != newOrientation))
	{
		auto dingPic = this->m_currentLevel->CreateDingImage(this->m_selectedDingID, newOrientation);
		auto needResetOrientation = this->m_selectedDingID != curDingID;
		myHUD->SelectDing(this->SelectedDing(), dingPic, needResetOrientation);
	}

	this->m_selectedDingOrientation = newOrientation;

	/* we might emplace objects */
	if (this->m_currentLevelEditorCellKnown)
	{
		this->m_IsErasing = myHUD->IsInEraserMode();
		this->m_IsOverwriting = myHUD->IsInOverwriteMode();
		this->m_selectedDingID = myHUD->SelectedDingID();
		if (this->PeekTouchdown())
		{
			bool needRedraw = false;
			if (this->m_IsErasing)
			{
				this->DoObliterateDing();
			}
			else 
			{
				this->ConsiderPlacement(this->m_keyShiftDown);
			}
		}
	}
}

void LevelEditor::SetControl(int detentCount, bool shiftKeyActive)
{
	/* mousewheeled */
	auto myHUD = static_cast<LevelEditorHUD*>(UserInterface::GetInstance().GetElement(blooDot::UIElement::LevelEditorHUD));
	if (myHUD != nullptr)
	{
		if (detentCount > 0)
		{
			this->DoRotate(shiftKeyActive, false);
		}
		else if (detentCount < 0)
		{
			this->DoRotate(shiftKeyActive, true);
		}
	}

	this->m_keyShiftDown = shiftKeyActive;
}

void LevelEditor::ConsiderPlacement(bool fillArea)
{
	if (this->m_currentLevelEditorCellKnown && this->m_selectedDingID > 0)
	{
		if (fillArea && m_lastPlacementPositionValid)
		{
			auto currentCellBackup = this->m_currentLevelEditorCell;
			auto xFrom = this->m_lastPlacementPosition.x > this->m_currentLevelEditorCell.x ? this->m_currentLevelEditorCell.x : this->m_lastPlacementPosition.x;
			auto yFrom = this->m_lastPlacementPosition.y > this->m_currentLevelEditorCell.y ? this->m_currentLevelEditorCell.y : this->m_lastPlacementPosition.y;
			auto xTo = this->m_lastPlacementPosition.x < this->m_currentLevelEditorCell.x ? this->m_currentLevelEditorCell.x : this->m_lastPlacementPosition.x;
			auto yTo = this->m_lastPlacementPosition.y < this->m_currentLevelEditorCell.y ? this->m_currentLevelEditorCell.y : this->m_lastPlacementPosition.y;
			for (auto y = yFrom; y <= yTo; ++y)
			{
				for (auto x = xFrom; x <= xTo; ++x)
				{
					this->m_currentLevelEditorCell.x = x;
					this->m_currentLevelEditorCell.y = y;
					this->DoPlaceDing();
				}
			}

			/* so we can comfortably continue to "connect the dots" */
			this->m_currentLevelEditorCell = currentCellBackup;
			this->m_lastPlacementPosition = currentCellBackup;
			this->m_lastPlacementPositionValid = true;
		}
		else
		{
			this->DoPlaceDing();
		}
	}
}

void LevelEditor::DoPlaceDing()
{
	if (this->m_currentLevelEditorCellKnown && this->m_selectedDingID > 0)
	{
		auto newCell = this->m_currentLevel->GetObjectAt(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, true);
		auto newDings = this->m_currentLevel->GetDing(this->m_selectedDingID);
		auto newDingID = newDings->ID();
		auto newDingLayer = newDings->GetPreferredLayer();
		auto curDings = newCell->GetDing(newDingLayer);
		if (curDings == nullptr || (this->m_IsOverwriting && curDings->ID() != newDingID))
		{
			auto neighborHood = this->m_currentLevel->GetNeighborConfigurationOf(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, newDingID, newDingLayer);
			if (newDings->CouldCoalesce())
			{
				newCell->Instantiate(newDings, neighborHood);
				this->ClumsyPackNeighborhoodOf(neighborHood, this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, newDingLayer, newDingID);
			}
			else
			{
				newCell->InstantiateInLayerFacing(newDingLayer, newDings, this->m_selectedDingOrientation);
			}

			this->RedrawSingleSquare(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, newDingLayer);
			this->m_lastPlacementPosition.x = this->m_currentLevelEditorCell.x;
			this->m_lastPlacementPosition.y = this->m_currentLevelEditorCell.y;
			this->m_lastPlacementPositionValid = true;
		}
	}
}

void LevelEditor::DoRotateObject(bool inverse)
{
	if (this->m_currentLevelEditorCellKnown)
	{
		auto curCell = this->m_currentLevel->GetObjectAt(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, false);
		if (curCell != nullptr)
		{
			auto topLayer = curCell->GetTopmostPopulatedLayer();
			auto curDings = curCell->GetDing(topLayer);
			if (curDings != nullptr && !curDings->CouldCoalesce())
			{
				curCell->RotateInPlace(topLayer, inverse);
				this->RedrawSingleSquare(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, topLayer);
			}
		}
	}
}

void LevelEditor::DoObliterateDing() 
{
	if (this->m_currentLevelEditorCellKnown)
	{
		Layers layerToCull;
		auto dingWeeded = this->m_currentLevel->WeedObjectAt(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, &layerToCull);
		if (dingWeeded != nullptr)
		{
			this->RedrawSingleSquare(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, layerToCull);
			if (dingWeeded->CouldCoalesce())
			{
				auto weededDingID = dingWeeded->ID();
				auto neighborHood = this->m_currentLevel->GetNeighborConfigurationOf(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, weededDingID, layerToCull);
				this->ClumsyPackNeighborhoodOf(neighborHood, this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, layerToCull, weededDingID);
			}
		}
	}
}

void LevelEditor::ClumsyPackNeighborhoodOf(ClumsyPacking::NeighborConfiguration neighborHood, unsigned aroundLevelX, unsigned aroundLevelY, Layers inLayer, unsigned dingID)
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

void LevelEditor::ClumsyPackNeighborhoodOf(unsigned aroundLevelX, unsigned aroundLevelY, Layers inLayer, unsigned dingID)
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

void LevelEditor::RedrawSingleSquare(unsigned levelX, unsigned levelY, Layers inLayer)
{
	auto sheetSize = this->m_currentLevel->GetSheetSizeUnits();
	auto intersectedSheetX = levelX / sheetSize.width;
	auto cellInSheetX = levelX % sheetSize.width;
	auto intersectedSheetY = levelY / sheetSize.height;
	auto cellInSheetY = levelY % sheetSize.height;
	auto placementSheet = this->GetSheet(intersectedSheetX, intersectedSheetY);
	placementSheet->RedrawSingleSquare(cellInSheetX, cellInSheetY, inLayer);
}

void LevelEditor::Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition)
{
#ifdef _DEBUG
	auto blitSheetCount = 0;
#endif
	m_d2dContext->SaveDrawingState(m_stateBlock.Get());
	m_d2dContext->BeginDraw();
	m_d2dContext->SetTransform(orientation2D);
	auto screenRect = D2D1::RectF(
		0,
		0,
		m_viewportSize.width,
		m_viewportSize.height
	);

	if (this->m_hoveringSheetNW != nullptr)
	{
		this->m_hoveringSheetNW->BlitToViewport();
#ifdef _DEBUG
		this->m_hoveringSheetNW->DebugDrawBorder(this->m_debugBorderBrush);
		++blitSheetCount;
#endif
	}

	if (this->m_hoveringSheetNE != nullptr)
	{
		this->m_hoveringSheetNE->BlitToViewport();
#ifdef _DEBUG
		this->m_hoveringSheetNE->DebugDrawBorder(this->m_debugBorderBrush);
		++blitSheetCount;
#endif
	}

	if (this->m_hoveringSheetSW != nullptr)
	{
		this->m_hoveringSheetSW->BlitToViewport();
#ifdef _DEBUG
		this->m_hoveringSheetSW->DebugDrawBorder(this->m_debugBorderBrush);
		++blitSheetCount;
#endif
	}

	if (this->m_hoveringSheetSE != nullptr)
	{
		this->m_hoveringSheetSE->BlitToViewport();
#ifdef _DEBUG
		this->m_hoveringSheetSE->DebugDrawBorder(this->m_debugBorderBrush);
		++blitSheetCount;
#endif
	}

	this->DrawLevelEditorRaster();
	HRESULT hr = m_d2dContext->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
	{
		DX::ThrowIfFailed(hr);
	}

	m_d2dContext->RestoreDrawingState(m_stateBlock.Get());

#ifdef _DEBUG
	if (blitSheetCount != this->m_lastBlitSheetCount)
	{
		const int bufferLength = 16;
		char16 str[bufferLength];
		int len = swprintf_s(str, bufferLength, L"%d", blitSheetCount);
		Platform::String^ string = ref new Platform::String(str, len);
		OutputDebugStringW(Platform::String::Concat(Platform::String::Concat(L"Number of sheets blitted: ", string), L"\r\n")->Data());
		this->m_lastBlitSheetCount = blitSheetCount;
	}
#endif
}

void LevelEditor::DrawLevelEditorRaster()
{
	D2D1_POINT_2F point0, point1;
	D2D1_RECT_F rect0;
	MFARGB colgrid = { 128, 128, 128, 128 };
	MFARGB colhigh = { 255, 0, 0, 255 };
	MFARGB coltuch = { 5, 151, 251,255 };
	MFARGB coleras = { 18, 7, 231, 255 };
	auto brgrid = m_Brushes.WannaHave(m_d2dContext, colgrid);
	auto cursor = m_Brushes.WannaHave(m_d2dContext, colhigh);
	auto touchdown = m_Brushes.WannaHave(m_d2dContext, coltuch);
	auto eraser = m_Brushes.WannaHave(m_d2dContext, coleras);
	auto brusherl = brgrid.Get();

	/* the next multiple of the grid height less than the viewport top */
	auto yAdjust = static_cast<float>(static_cast<unsigned>(m_viewportOffset.y) % static_cast<unsigned>(blooDot::Consts::SQUARE_HEIGHT));
	auto xAdjust = static_cast<float>(static_cast<unsigned>(m_viewportOffset.x) % static_cast<unsigned>(blooDot::Consts::SQUARE_WIDTH));
	if (this->m_isGridShown)
	{
		for (
			int y = -static_cast<int>(yAdjust);
			y < static_cast<int>(m_viewportSize.height + blooDot::Consts::SQUARE_HEIGHT);
			y += static_cast<int>(blooDot::Consts::SQUARE_HEIGHT)
			)
		{
			point0.x = (float)0;
			point0.y = (float)y;
			point1.x = (float)static_cast<int>(m_viewportSize.width);
			point1.y = (float)y;
			m_d2dContext->DrawLine(point0, point1, brusherl, 1.0F, NULL);
		}

		for (
			int x = -static_cast<int>(xAdjust);
			x < static_cast<int>(m_viewportSize.width + blooDot::Consts::SQUARE_WIDTH);
			x += static_cast<int>(blooDot::Consts::SQUARE_WIDTH)
			)
		{
			point0.x = (float)x;
			point0.y = (float)0;
			point1.x = (float)x;
			point1.y = (float)static_cast<int>(m_viewportSize.height);
			m_d2dContext->DrawLine(point0, point1, brusherl, 1.0F, NULL);
		}
	}

	auto halfSquareX = blooDot::Consts::SQUARE_WIDTH / 2.0F;
	auto halfSquareY = blooDot::Consts::SQUARE_HEIGHT / 2.0F;
	auto pointX = this->m_pointerPosition.x;
	auto pointY = this->m_pointerPosition.y;

	/* the cursor aka the mouse may hover over one grid square */
	auto localSquareX = static_cast<unsigned>((pointX + xAdjust) / blooDot::Consts::SQUARE_WIDTH);
	auto localSquareY = static_cast<unsigned>((pointY + yAdjust) / blooDot::Consts::SQUARE_HEIGHT);
	rect0.left = localSquareX * blooDot::Consts::SQUARE_WIDTH - xAdjust;
	rect0.top = localSquareY * blooDot::Consts::SQUARE_HEIGHT - yAdjust;
	rect0.right = rect0.left + blooDot::Consts::SQUARE_WIDTH;
	rect0.bottom = rect0.top + blooDot::Consts::SQUARE_HEIGHT;
	if (this->PeekTouchdown())
	{		
		brusherl = touchdown.Get();
	}
	else
	{
		if (this->m_IsErasing)
		{
			brusherl = eraser.Get();
		}
		else
		{			
			brusherl = cursor.Get();
		}
	}

	/* we use this to remember the current cursor cell, just because we can
	 * (and, because it is smart since we already have the values here) */
	this->m_d2dContext->DrawRectangle(rect0, brusherl, 1.0F, NULL);
	this->m_currentLevelEditorCell.x = this->m_viewportOffsetSquares.x + localSquareX;
	this->m_currentLevelEditorCell.y = this->m_viewportOffsetSquares.y + localSquareY;
	if (!this->m_currentLevelEditorCellKnown)
	{
		this->m_currentLevelEditorCellKnown = this->m_currentLevelEditorCell.x > 0 || this->m_currentLevelEditorCell.y > 0;
	}

	/* show where we are */
	if (!m_lastActiveGridPositionValid || this->m_lastActiveGridPosition.x != this->m_currentLevelEditorCell.x || this->m_lastActiveGridPosition.y != this->m_currentLevelEditorCell.y)
	{
		this->m_textLayout.Reset();
		this->m_textLayout = nullptr;
		WCHAR buffer[64];
		swprintf_s(
			buffer,
			L"[%d,%d]",
			this->m_currentLevelEditorCell.x,
			this->m_currentLevelEditorCell.y
		);

		this->CreateTextLayout(&rect0, ref new Platform::String(buffer));
		this->m_lastActiveGridPosition.x = this->m_currentLevelEditorCell.x;
		this->m_lastActiveGridPosition.y = this->m_currentLevelEditorCell.y;
		this->m_lastActiveGridPositionValid = true;
	}

	this->m_d2dContext->DrawTextLayout(
		D2D1::Point2F(
			rect0.left,
			rect0.top
		),
		this->m_textLayout.Get(),
		this->m_textColorBrush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
	);
}

void LevelEditor::CreateTextLayout(D2D1_RECT_F* rect, Platform::String^ text)
{
	if (this->m_textLayout == nullptr)
	{
		auto dwriteFactory = UserInterface::GetDWriteFactory();
		DX::ThrowIfFailed(
			dwriteFactory->CreateTextLayout(
				text->Data(),
				text->Length(),
				this->m_textStyle.GetTextFormat(),
				rect->right - rect->left,
				rect->bottom - rect->top,
				&this->m_textLayout
			)
		);

		this->m_textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}
}

Dings* LevelEditor::SelectedDing()
{
	if (this->m_selectedDingID > 0)
	{
		return this->m_currentLevel->GetDing(m_selectedDingID);
	}

	return nullptr;
}

void LevelEditor::SelectDingForPlacement(unsigned dingID)
{
	this->m_selectedDingID = this->m_currentLevel->ConfirmDingID(dingID);
}

void LevelEditor::SelectNextDingForPlacement()
{
	auto nextDing = this->m_currentLevel->GetNextDingID(this->m_selectedDingID);
	if (nextDing > 0)
	{
		this->SelectDingForPlacement(nextDing);
	}
}

void LevelEditor::SelectPreviousDingForPlacement()
{
	auto prevDing = this->m_currentLevel->GetPreviousDingID(this->m_selectedDingID);
	if (prevDing > 0)
	{
		this->SelectDingForPlacement(prevDing);
	}
}

void LevelEditor::DoSetScrollLock(bool scrollLocked)
{
	auto myHUD = static_cast<LevelEditorHUD*>(UserInterface::GetInstance().GetElement(blooDot::UIElement::LevelEditorHUD));
	if (myHUD != nullptr)
	{
		myHUD->SetScrollLock(scrollLocked);
	}
}

void LevelEditor::DoToggleGrid()
{
	auto myHUD = static_cast<LevelEditorHUD*>(UserInterface::GetInstance().GetElement(blooDot::UIElement::LevelEditorHUD));
	if (myHUD != nullptr)
	{
		myHUD->ToggleGrid();
	}
}

void LevelEditor::DoRotate(bool affectPlacement, bool inverse)
{
	if (affectPlacement)
	{
		this->DoRotateObject(inverse);
	}
	else
	{
		auto myHUD = static_cast<LevelEditorHUD*>(UserInterface::GetInstance().GetElement(blooDot::UIElement::LevelEditorHUD));
		if (myHUD != nullptr)
		{
			if (inverse)
			{
				myHUD->RotateInverse();
			}
			else
			{
				myHUD->Rotate();
			}
		}
	}
}
