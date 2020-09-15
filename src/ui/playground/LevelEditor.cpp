#include "..\..\PreCompiledHeaders.h"
#include "..\..\dx\DirectXHelper.h"
#include "..\UserInterface.h"
#include "..\DialogDingSheet.h"

LevelEditor::LevelEditor() : WorldScreenBase() 
{
	this->m_selectedDingID = Dings::DingIDs::Mauer;
	this->m_selectedDingOrientation = Facings::Shy;
	this->m_isGridShown = true;
	this->m_isDingSheetShown = false;
	this->m_lastPlacementPositionValid = false;
	this->m_lastActiveGridPositionValid = false;
	this->m_keyShiftDown = false;
}

LevelEditor::~LevelEditor()
{	
	this->m_textColorBrush.Reset();
	this->m_coverBrush.Reset();
	this->m_chromeBrush.Reset();
	this->m_blackBrush.Reset();
}

void LevelEditor::Initialize(_In_ std::shared_ptr<Audio> audioEngine, _In_ std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	WorldScreenBase::Initialize(audioEngine, deviceResources);
	DX::ThrowIfFailed(deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &this->m_textColorBrush));
	this->m_textColorBrush->SetOpacity(m_textColorBrush->GetOpacity() * blooDot::Consts::GOLDEN_RATIO);
	DX::ThrowIfFailed(deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &this->m_blackBrush));
	DX::ThrowIfFailed(deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::PaleGoldenrod), &this->m_chromeBrush));
	DX::ThrowIfFailed(deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GhostWhite), &this->m_coverBrush));
	this->m_coverBrush->SetOpacity(m_coverBrush->GetOpacity() * blooDot::Consts::INVERSE_GOLDEN_RATIO);
	this->m_textStyle.SetFontName(L"Segoe UI");
	this->m_textStyle.SetFontSize(11.0f);
	this->m_textStyle.SetFontWeight(DWRITE_FONT_WEIGHT_LIGHT);
	this->m_textStyle.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
}

void LevelEditor::Update(float timeTotal, float timeDelta)
{
	WorldScreenBase::Update(timeTotal, timeDelta);

	/* we would have a HUD showing the currently selected toolbox ding,
	 * and controlling the visibility of the ding sheet modal dialog */
	auto myHUD = static_cast<LevelEditorHUD*>(UserInterface::GetInstance().GetElement(blooDot::UIElement::LevelEditorHUD));
	auto newOrientation = myHUD->SelectedDingOrientation();
	auto curDingID = myHUD->SelectedDingID();
	this->m_isGridShown = myHUD->IsGridShown();
	auto newDingSheetShown = myHUD->IsDingSheetShown();
	if (this->m_isDingSheetShown != newDingSheetShown)
	{
		this->m_isDingSheetShown = newDingSheetShown;
		auto dlgDingSheet = static_cast<DialogDingSheet*>(UserInterface::GetInstance().GetElement(blooDot::UIElement::DingSheetDialog));
		if (newDingSheetShown && this->m_currentLevel != nullptr)
		{
			dlgDingSheet->SetContent(this->m_currentLevel);
		}

		dlgDingSheet->SetVisible(newDingSheetShown);
	}

	if (this->m_isDingSheetShown)
	{
		auto dialogCommand = myHUD->DequeDingSheetCommand();
		if (dialogCommand != blooDot::DialogCommand::None)
		{
			this->ProcessDialogCommand(blooDot::UIElement::DingSheetDialog, dialogCommand);
		}
	}

	if (this->m_selectedDingID > Dings::DingIDs::Void && (this->m_selectedDingID != curDingID || this->m_selectedDingOrientation != newOrientation))
	{
		Microsoft::WRL::ComPtr<ID2D1Bitmap> dingPic;
		auto selectedDing = this->SelectedDing();
		if (selectedDing->IsMob())
		{
			dingPic = this->m_currentLevel->CreateMobImage(this->m_selectedDingID, newOrientation);
		}
		else
		{
			dingPic = this->m_currentLevel->CreateDingImage(this->m_selectedDingID, newOrientation);
		}

		auto needResetOrientation = this->m_selectedDingID != curDingID;
		myHUD->SelectDing(selectedDing, dingPic, needResetOrientation);
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

void LevelEditor::ProcessDialogCommand(blooDot::UIElement dialogElement, blooDot::DialogCommand dialogCommand)
{
	if (dialogCommand != blooDot::DialogCommand::None)
	{
		auto dialogWindow = static_cast<DialogOverlay*>(UserInterface::GetInstance().GetElement(dialogElement));
		if (dialogWindow != nullptr)
		{
			dialogWindow->ScheduleDialogCommand(dialogCommand);
		}
	}
}

void LevelEditor::ConsiderPlacement(bool fillArea)
{
	if (this->m_currentLevelEditorCellKnown && this->m_selectedDingID > Dings::DingIDs::Void)
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
	if (this->m_currentLevelEditorCellKnown && this->m_selectedDingID > Dings::DingIDs::Void)
	{
		auto newCell = this->m_currentLevel->GetBlocksAt(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, true);
		auto newDings = this->m_currentLevel->GetDing(this->m_selectedDingID);
		auto newDingID = newDings->ID();
		auto newDingLayer = newDings->GetPreferredLayer();
		auto curObject = newCell->GetObject(newDingLayer);
		auto curDings = curObject == nullptr ? nullptr : curObject->GetDing();
		if (curDings == nullptr || (this->m_IsOverwriting && curDings->ID() != newDingID))
		{
			auto neighborHood = this->m_currentLevel->GetNeighborConfigurationOf(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, newDingID, newDingLayer);
			if (newDings->CouldCoalesce())
			{
				newCell->Instantiate(this->m_currentLevel, newDings, neighborHood);
				this->ClumsyPackNeighborhoodOf(neighborHood, this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, newDingLayer, newDingID);
			}
			else
			{
				newCell->InstantiateInLayerFacing(this->m_currentLevel, newDingLayer, newDings, this->m_selectedDingOrientation);
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
		auto curCell = this->m_currentLevel->GetBlocksAt(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, false);
		if (curCell != nullptr)
		{
			auto topLayer = curCell->GetTopmostPopulatedLayer();
			auto curDings = curCell->GetObject(topLayer)->GetDing();
			if (curDings != nullptr && !curDings->CouldCoalesce())
			{
				curCell->GetObject(topLayer)->RotateInPlace(inverse);
				this->RedrawSingleSquare(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, topLayer);
			}
		}
	}
}

void LevelEditor::DoObliterateDing() 
{
	if (this->m_currentLevelEditorCellKnown)
	{
		this->ObliterateObject(this->m_currentLevelEditorCell);
	}
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

	/* -!- don't blit if entirely off viewport. no need. */
	if (WORLDSHEET_NW != nullptr)
	{
		WORLDSHEET_NW->BlitToViewport();
#ifdef _DEBUG
		WORLDSHEET_NW->DebugDrawBorder(this->m_debugBorderBrush);
		++blitSheetCount;
#endif
	}

	if (WORLDSHEET_NE != nullptr)
	{
		WORLDSHEET_NE->BlitToViewport();
#ifdef _DEBUG
		WORLDSHEET_NE->DebugDrawBorder(this->m_debugBorderBrush);
		++blitSheetCount;
#endif
	}

	if (WORLDSHEET_SW != nullptr)
	{
		WORLDSHEET_SW->BlitToViewport();
#ifdef _DEBUG
		WORLDSHEET_SW->DebugDrawBorder(this->m_debugBorderBrush);
		++blitSheetCount;
#endif
	}

	if (WORLDSHEET_SE != nullptr)
	{
		WORLDSHEET_SE->BlitToViewport();
#ifdef _DEBUG
		WORLDSHEET_SE->DebugDrawBorder(this->m_debugBorderBrush);
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
	auto brgrid = this->m_Brushes->WannaHave(m_d2dContext, colgrid);
	auto cursor = m_Brushes->WannaHave(m_d2dContext, colhigh);
	auto touchdown = m_Brushes->WannaHave(m_d2dContext, coltuch);
	auto eraser = m_Brushes->WannaHave(m_d2dContext, coleras);
	auto brusherl = brgrid.Get();

	/* the next multiple of the grid height less than the viewport top */
	auto yAdjust = static_cast<float>(static_cast<unsigned>(this->m_viewportOffset.y) % static_cast<unsigned>(blooDot::Consts::SQUARE_HEIGHT));
	auto xAdjust = static_cast<float>(static_cast<unsigned>(this->m_viewportOffset.x) % static_cast<unsigned>(blooDot::Consts::SQUARE_WIDTH));
	if (this->m_isGridShown)
	{
		for (
			int y = -static_cast<int>(yAdjust);
			y < static_cast<int>(this->m_viewportSize.height + blooDot::Consts::SQUARE_HEIGHT);
			y += static_cast<int>(blooDot::Consts::SQUARE_HEIGHT)
			)
		{
			point0.x = (float)0;
			point0.y = (float)y;
			point1.x = (float)static_cast<int>(this->m_viewportSize.width);
			point1.y = (float)y;
			this->m_d2dContext->DrawLine(point0, point1, brusherl, 1.0F, NULL);
		}

		for (
			int x = -static_cast<int>(xAdjust);
			x < static_cast<int>(this->m_viewportSize.width + blooDot::Consts::SQUARE_WIDTH);
			x += static_cast<int>(blooDot::Consts::SQUARE_WIDTH)
			)
		{
			point0.x = (float)x;
			point0.y = (float)0;
			point1.x = (float)x;
			point1.y = (float)static_cast<int>(this->m_viewportSize.height);
			this->m_d2dContext->DrawLine(point0, point1, brusherl, 1.0F, NULL);
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

std::shared_ptr<Dings> LevelEditor::SelectedDing()
{
	if (this->m_selectedDingID > Dings::DingIDs::Void)
	{
		return this->m_currentLevel->GetDing(m_selectedDingID);
	}

	return nullptr;
}

void LevelEditor::SelectDingForPlacement(Dings::DingIDs dingID)
{
	this->m_selectedDingID = this->m_currentLevel->ConfirmDingID(dingID);
}

void LevelEditor::SelectNextDingForPlacement()
{
	auto nextDing = this->m_currentLevel->GetNextDingID(this->m_selectedDingID);
	if (nextDing > Dings::DingIDs::Void)
	{
		this->SelectDingForPlacement(nextDing);
	}
}

void LevelEditor::SelectPreviousDingForPlacement()
{
	auto prevDing = this->m_currentLevel->GetPreviousDingID(this->m_selectedDingID);
	if (prevDing > Dings::DingIDs::Void)
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

void LevelEditor::DoToggleDingSheet()
{
	auto myHUD = static_cast<LevelEditorHUD*>(UserInterface::GetInstance().GetElement(blooDot::UIElement::LevelEditorHUD));
	if (myHUD != nullptr)
	{
		myHUD->ToggleDingSheet();
	}
}

void LevelEditor::SendDialogCommand(blooDot::DialogCommand dialogCommand)
{
	auto myHUD = static_cast<LevelEditorHUD*>(UserInterface::GetInstance().GetElement(blooDot::UIElement::LevelEditorHUD));
	if (myHUD != nullptr)
	{
		if (this->m_isDingSheetShown)
		{
			myHUD->SetDingSheetCommand(dialogCommand);
		}
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

void LevelEditor::ImportIntoCurrentLevel(std::shared_ptr<Level> sourceLevel)
{
	auto anyChanges = false;
	if (this->m_currentLevelEditorCellKnown)
	{
		unsigned targetX, targetY;
		auto importBounds = sourceLevel->DeterminePopulatedAreaBounds();
		for (auto originY = importBounds.top; originY <= importBounds.bottom; ++originY)
		{
			targetY = this->m_currentLevelEditorCell.y + originY - importBounds.top;
			for (auto originX = importBounds.left; originX <= importBounds.right; ++originX)
			{
				auto originObject = sourceLevel->GetBlocksAt(originX, originY, false);
				if (originObject != nullptr)
				{
					targetX = this->m_currentLevelEditorCell.x + originX - importBounds.left;
					auto newCell = this->m_currentLevel->GetBlocksAt(targetX, targetY, true);
					if (newCell == nullptr)
					{
						/* not placeable in target (out of bounds, most likely --> gets clipped) */
					}
					else
					{
						auto originLayers = originObject->GetLayers();
						if (originLayers & Layers::Floor)
						{
							this->CloneObjectToCell(originObject, newCell, Layers::Floor, targetX, targetY);
							anyChanges = true;
						}

						if (originLayers & Layers::Walls)
						{
							this->CloneObjectToCell(originObject, newCell, Layers::Walls, targetX, targetY);
							anyChanges = true;
						}

						if (originLayers & Layers::Rooof)
						{
							this->CloneObjectToCell(originObject, newCell, Layers::Rooof, targetX, targetY);
							anyChanges = true;
						}
					}
				}
			}
		}

		if (anyChanges)
		{
			/* forces a redraw by simple invalidating the entire viewport */
			this->InvalidateLevelViewport();
		}
	}
}

void LevelEditor::CloneObjectToCell(std::shared_ptr<Blocks> sourceCell, std::shared_ptr<Blocks> targetCell, Layers inLayer, unsigned targetX, unsigned targetY)
{
	auto newDingID = sourceCell->GetObject(inLayer)->GetDing()->ID();
	auto newDings = this->m_currentLevel->GetDing(newDingID);
	auto curDings = targetCell->GetObject(inLayer)->GetDing();
	if (curDings == nullptr || (this->m_IsOverwriting && curDings->ID() != newDingID))
	{
		auto intendedFacing = sourceCell->GetObject(inLayer)->PlacementFacing();
		auto neighborHood = this->m_currentLevel->GetNeighborConfigurationOf(targetX, targetY, newDingID, inLayer);
		targetCell->InstantiateInLayerFacing(this->m_currentLevel, inLayer, newDings, intendedFacing);
		if (newDings->CouldCoalesce())
		{
			this->ClumsyPackNeighborhoodOf(neighborHood, targetX, targetY, inLayer, newDingID);
		}
	}
}