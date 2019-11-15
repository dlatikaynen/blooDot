#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

LevelEditor::~LevelEditor()
{
}

void LevelEditor::Update(float timeTotal, float timeDelta)
{
	WorldScreenBase::Update(timeTotal, timeDelta);
	
	/* we might emplace objects */
	if (this->m_currentLevelEditorCellKnown && this->PeekTouchdown())
	{
		//this->PopTouchdown();
		auto newCell = this->m_currentLevel->GetObjectAt(this->m_currentLevelEditorCell.x, this->m_currentLevelEditorCell.y, true);
		auto curDings = newCell->GetDings();
		auto newDings = this->m_currentLevel->GetDing(1);
		if (curDings == nullptr || curDings->ID() != newDings->ID())
		{
			newCell->Instantiate(newDings);
			/* draw the object onto the sheet immediately */
			auto sheetSize = this->m_currentLevel->GetSheetSizeUnits();
			auto intersectedSheetX = this->m_currentLevelEditorCell.x / sheetSize.width;
			auto cellInSheetX = this->m_currentLevelEditorCell.x % sheetSize.width;
			auto intersectedSheetY = this->m_currentLevelEditorCell.y / sheetSize.height;
			auto cellInSheetY = this->m_currentLevelEditorCell.y % sheetSize.height;
			auto placementSheet = this->GetSheet(intersectedSheetX, intersectedSheetY);
			placementSheet->RedrawSingleSquare(cellInSheetX, cellInSheetY);
		}
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
	char16 str[20];
	int len = swprintf_s(str, sizeof(str) / sizeof(char16), L"%d\r\n", blitSheetCount);
	Platform::String^ string = ref new Platform::String(str, len);
	OutputDebugStringW(Platform::String::Concat(L"Number of sheets blitted: ", string)->Data());
#endif
}

void LevelEditor::DrawLevelEditorRaster()
{
	D2D1_POINT_2F point0, point1;
	D2D1_RECT_F rect0;
	MFARGB color = { 128, 128, 128, 128 };
	MFARGB colhigh = { 255, 0, 0, 255 };
	MFARGB coltuch = { 5, 151, 251,255 };
	Microsoft::WRL::ComPtr<ID2D1Brush> brush = m_Brushes.WannaHave(m_d2dContext, color);
	Microsoft::WRL::ComPtr<ID2D1Brush> highlight = m_Brushes.WannaHave(m_d2dContext, colhigh);
	Microsoft::WRL::ComPtr<ID2D1Brush> touchdown = m_Brushes.WannaHave(m_d2dContext, coltuch);

	auto brusherl = brush.Get();

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

	m_d2dContext->DrawRectangle(D2D1::RectF(
		pointX-1.0F,
		pointY-1.0F,
		pointX+1.0F,
		pointY+1.0F
	), highlight.Get(), 1.0F, NULL);

	/* the cursor aka the mouse may hover over one grid square */
	auto localSquareX = static_cast<unsigned>((pointX + xAdjust) / blooDot::Consts::SQUARE_WIDTH);
	auto localSquareY = static_cast<unsigned>((pointY + yAdjust) / blooDot::Consts::SQUARE_HEIGHT);
	rect0.left = localSquareX * blooDot::Consts::SQUARE_WIDTH - xAdjust;
	rect0.top = localSquareY * blooDot::Consts::SQUARE_HEIGHT - yAdjust;
	rect0.right = rect0.left + blooDot::Consts::SQUARE_WIDTH;
	rect0.bottom = rect0.top + blooDot::Consts::SQUARE_HEIGHT;
	m_d2dContext->DrawRectangle(rect0, this->PeekTouchdown() ? touchdown.Get() : highlight.Get(), 1.0F, NULL);

	/* we use this to remember the current cursor cell, just because we can
	 * (and, because it is smart since we already have the values here) */
	this->m_currentLevelEditorCell.x = this->m_viewportOffsetSquares.x + localSquareX;
	this->m_currentLevelEditorCell.y = this->m_viewportOffsetSquares.y + localSquareY;
	if (!this->m_currentLevelEditorCellKnown)
	{
		this->m_currentLevelEditorCellKnown = this->m_currentLevelEditorCell.x > 0 || this->m_currentLevelEditorCell.y > 0;
	}
}