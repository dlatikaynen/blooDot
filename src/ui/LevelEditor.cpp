#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

LevelEditor::LevelEditor() : WorldScreenBase() 
{
	this->m_selectedDingID = 1;
}

LevelEditor::~LevelEditor()
{
}

void LevelEditor::Update(float timeTotal, float timeDelta)
{
	WorldScreenBase::Update(timeTotal, timeDelta);

	/* we would have a HUD showing the currently selected toolbox ding */
	auto myHUD = static_cast<LevelEditorHUD*>(UserInterface::GetInstance().GetElement(blooDot::UIElement::LevelEditorHUD));
	if (this->m_selectedDingID > 0 && this->m_selectedDingID != myHUD->SelectedDingID())
	{
		auto dingPic = this->m_currentLevel->CreateDingImage(this->m_selectedDingID);
		myHUD->SelectDing(this->SelectedDing(), dingPic);
	}

	/* we might emplace objects */
	if (this->m_currentLevelEditorCellKnown)
	{
		this->m_IsErasing = myHUD->IsInEraserMode();
		this->m_selectedDingID = myHUD->SelectedDingID();

		if (this->PeekTouchdown())
		{
			bool needRedraw = false;
			if (this->m_IsErasing)
			{
				needRedraw = this->m_currentLevel->WeedObjectAt(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y);
			}
			else if (this->m_selectedDingID > 0)
			{
				auto inLayer = Layers::Walls;
				auto newCell = this->m_currentLevel->GetObjectAt(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, true);
				auto curDings = newCell->GetDing(inLayer);
				auto newDings = this->m_currentLevel->GetDing(this->m_selectedDingID);
				auto newDingID = newDings->ID();
				if (curDings == nullptr || curDings->ID() != newDingID)
				{
					auto neighborHood = this->m_currentLevel->GetNeighborConfigurationOf(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, newDingID, inLayer);
					newCell->Instantiate(newDings, neighborHood);
					if (newDings->CouldCoalesce())
					{
						this->ClumsyPackNeighborhoodOf(neighborHood, this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, inLayer, newDingID);
					}

					needRedraw = true;
				}
			}

			if (needRedraw)
			{
				/* draw the object onto the sheet immediately */
				this->RedrawSingleSquare(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y);
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
			if (centerObject->PlacementFacing() != shouldBeFacing)
			{
				centerObject->AdjustFacing(inLayer, shouldBeFacing);
				this->RedrawSingleSquare(aroundLevelX, aroundLevelY);
				this->ClumsyPackNeighborhoodOf(neighborHood, aroundLevelX, aroundLevelY, inLayer, dingID);
			}
		}
	}
}

void LevelEditor::RedrawSingleSquare(unsigned levelX, unsigned levelY)
{
	auto sheetSize = this->m_currentLevel->GetSheetSizeUnits();
	auto intersectedSheetX = levelX / sheetSize.width;
	auto cellInSheetX = levelX % sheetSize.width;
	auto intersectedSheetY = levelY / sheetSize.height;
	auto cellInSheetY = levelY % sheetSize.height;
	auto placementSheet = this->GetSheet(intersectedSheetX, intersectedSheetY);
	placementSheet->RedrawSingleSquare(cellInSheetX, cellInSheetY);
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

	for (
		int y = -yAdjust;
		y < static_cast<int>(m_viewportSize.height + blooDot::Consts::SQUARE_HEIGHT);
		y += static_cast<int>(blooDot::Consts::SQUARE_HEIGHT)
		)
	{
		point0.x = 0;
		point0.y = y;
		point1.x = static_cast<int>(m_viewportSize.width);
		point1.y = y;
		m_d2dContext->DrawLine(point0, point1, brusherl, 1.0F, NULL);
	}

	for (
		int x = -xAdjust;
		x < static_cast<int>(m_viewportSize.width + blooDot::Consts::SQUARE_WIDTH);
		x += static_cast<int>(blooDot::Consts::SQUARE_WIDTH)
		)
	{
		point0.x = x;
		point0.y = 0;
		point1.x = x;
		point1.y = static_cast<int>(m_viewportSize.height);
		m_d2dContext->DrawLine(point0, point1, brusherl, 1.0F, NULL);
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

	this->m_d2dContext->DrawRectangle(rect0, brusherl, 1.0F, NULL);

	/* we use this to remember the current cursor cell, just because we can
	 * (and, because it is smart since we already have the values here) */
	this->m_currentLevelEditorCell.x = this->m_viewportOffsetSquares.x + localSquareX;
	this->m_currentLevelEditorCell.y = this->m_viewportOffsetSquares.y + localSquareY;
	if (!this->m_currentLevelEditorCellKnown)
	{
		this->m_currentLevelEditorCellKnown = this->m_currentLevelEditorCell.x > 0 || this->m_currentLevelEditorCell.y > 0;
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
