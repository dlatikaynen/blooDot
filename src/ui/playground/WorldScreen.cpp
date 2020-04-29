#include "..\..\PreCompiledHeaders.h"
#include "..\UserInterface.h"
#include "..\..\dx\DirectXHelper.h"

WorldScreen::WorldScreen() : WorldScreenBase()
{

}

WorldScreen::~WorldScreen()
{	

}

void WorldScreen::Initialize(_In_ std::shared_ptr<DX::DeviceResources>&	deviceResources)
{
	WorldScreenBase::Initialize(deviceResources);
	/* player(s) positions, and ultimately initial position in level are determined from level metadata */
	if (this->m_playerData.empty() && this->m_currentLevel != nullptr)
	{
		auto player1 = new Player();
		player1->Name = L"Nepomuk der Nasenbär";
		player1->PositionSquare = D2D1::Point2U(355, 358);
		player1->PlaceInLevel(this->m_currentLevel);

		auto dingOnSheet = this->m_currentLevel->GetDing(Dings::DingIDs::Player)->GetSheetPlacement(Facings::East);
		player1->SpriteSourceRect = D2D1::RectF(dingOnSheet.x * 49.0f, dingOnSheet.y * 49.0f, dingOnSheet.x * 49.0f + 49.0f, dingOnSheet.y * 49.0f + 49.0f);
		this->m_playerData.push_back(player1);
	}
}

void WorldScreen::SetControl(DirectX::XMFLOAT2 pointerPosition, TouchMap* touchMap, bool shiftKeyActive, bool left, bool right, bool up, bool down, float scrollDeltaX, float scrollDeltaY)
{
	this->m_pointerPosition.x = pointerPosition.x;
	this->m_pointerPosition.y = pointerPosition.y;
	this->m_touchMap = touchMap;
	this->m_isMoving = Facings::Shy;
	this->m_keyShiftDown = shiftKeyActive;
	auto keyboardPlayer = *this->m_playerData.begin();
	if (left)
	{
		keyboardPlayer->Facing = Facings::West;
		keyboardPlayer->PushX(-0.15f, 0.01f, 1.0f, 0.05f);
	}
	else if (right)
	{
		keyboardPlayer->Facing = Facings::East;
		keyboardPlayer->PushX(0.15f, 0.01f, 1.0f, 0.05f);
	}
	else if (down)
	{
		keyboardPlayer->Facing = Facings::South;
		keyboardPlayer->PushY(0.15f, 0.01f, 1.0f, 0.05f);
	}
	else if (up)
	{
		keyboardPlayer->Facing = Facings::North;
		keyboardPlayer->PushY(-0.15f, 0.01f, 1.0f, 0.05f);
	}
}

// mousewheeled
void WorldScreen::SetControl(int detentCount, bool shiftKeyActive)
{

}

void WorldScreen::Update(float timeTotal, float timeDelta)
{
	WorldScreenBase::Update(timeTotal, timeDelta);

	/* update player positions */
	for (auto mob = this->m_playerData.begin(); mob != this->m_playerData.end(); ++mob)
	{		
		(*mob)->Update();
	}
}

void WorldScreen::Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition)
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

	/* blit all floor regions */
	if (this->m_hoveringSheetNW != nullptr)
	{
		this->m_hoveringSheetNW->BlitToViewportFloor();
	}

	if (this->m_hoveringSheetNE != nullptr)
	{
		this->m_hoveringSheetNE->BlitToViewportFloor();
	}

	if (this->m_hoveringSheetSW != nullptr)
	{
		this->m_hoveringSheetSW->BlitToViewportFloor();
	}

	if (this->m_hoveringSheetSE != nullptr)
	{
		this->m_hoveringSheetSE->BlitToViewportFloor();
	}

	/* blit all mob-level sprites */
	this->RenderSprites();

	/* blit all wall and rooof parts */
	if (this->m_hoveringSheetNW != nullptr)
	{
		this->m_hoveringSheetNW->BlitToViewportWallsRooof();
	}

	if (this->m_hoveringSheetNE != nullptr)
	{
		this->m_hoveringSheetNE->BlitToViewportWallsRooof();
	}

	if (this->m_hoveringSheetSW != nullptr)
	{
		this->m_hoveringSheetSW->BlitToViewportWallsRooof();
	}

	if (this->m_hoveringSheetSE != nullptr)
	{
		this->m_hoveringSheetSE->BlitToViewportWallsRooof();
	}

	HRESULT hr = m_d2dContext->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
	{
		DX::ThrowIfFailed(hr);
	}

	this->m_d2dContext->RestoreDrawingState(m_stateBlock.Get());
}

void WorldScreen::RenderSprites()
{
	auto spriteMap = this->m_currentLevel->GetDingSheetBmp();
	for (auto mob = this->m_playerData.begin(); mob != this->m_playerData.end(); ++mob)
	{
		auto screenRect = D2D1::RectF(
			(*mob)->Position.left - this->m_viewportOffset.x,
			(*mob)->Position.top - this->m_viewportOffset.y,
			(*mob)->Position.right - this->m_viewportOffset.x,
			(*mob)->Position.bottom - this->m_viewportOffset.y
		);

		this->m_d2dContext->DrawBitmap(
			this->m_currentLevel->GetDingSheetBmp(), 
			screenRect,
			1.0f, 
			D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, 
			(*mob)->SpriteSourceRect
		);

#ifdef _DEBUG
		auto gridPosition = (*mob)->PositionSquare;
		auto gridLocked = D2D1::RectF(
			gridPosition.x * blooDot::Consts::SQUARE_WIDTH - this->m_viewportOffset.x,
			gridPosition.y * blooDot::Consts::SQUARE_HEIGHT - this->m_viewportOffset.y,
			0,
			0
		);

		gridLocked.right = gridLocked.left + blooDot::Consts::SQUARE_WIDTH;
		gridLocked.bottom = gridLocked.top + blooDot::Consts::SQUARE_HEIGHT;
		this->m_d2dContext->DrawRectangle(gridLocked, this->m_debugBorderBrush.Get(), 0.7F);
#endif
	}
}