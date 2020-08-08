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
		player1->InitializeIn(
			L"Nepomuk der Nasenbär",
			this->m_currentLevel,
			355,
			358,
			Facings::East
		);

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
	auto keyboardPlayer = *this->m_playerData.begin();
	keyboardPlayer->Facing = Dings::RotateMobFine(keyboardPlayer->Facing, shiftKeyActive);
}

void WorldScreen::SetControl(bool triggershoot)
{
	if (triggershoot && !this->m_shooting)
	{
		auto Player = *this->m_playerData.begin();
		this->m_shooting = true;
		this->m_shootx = Player->Position.left + blooDot::Consts::SQUARE_WIDTH / 2.0f;
		this->m_shooty = Player->Position.top + blooDot::Consts::SQUARE_HEIGHT / 2.0f;
		this->m_shoot_directionX = 4.5f * cos(static_cast<float>(2.0f * M_PI / 16.0f * Player->m_Orientation) - M_PI / 2.0f);
		this->m_shoot_directionY = 4.5f * sin(static_cast<float>(2.0f * M_PI / 16.0f * Player->m_Orientation) - M_PI / 2.0f);
		this->m_blockstravelled = 0;
	}
}



void WorldScreen::Update(float timeTotal, float timeDelta)
{	
	/* update player positions */
	for (auto mob = this->m_playerData.begin(); mob != this->m_playerData.end(); ++mob)
	{		
		auto Player = (*mob);
		Player->Update();
	}

	if (this->m_shooting)
	{
		this->m_shootx += this->m_shoot_directionX;
		this->m_shooty += this->m_shoot_directionY;
		if (this->m_blockstravelled++ > (196))
		{
			this->m_shooting = false;
		}
	}

	/* scroll the viewport through the world, essentially */
	WorldScreenBase::Update(timeTotal, timeDelta);
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

	/* most particles are below the player's head */
	if (this->m_shooting)
	{
		this->m_d2dContext->FillEllipse(D2D1::Ellipse(D2D1::Point2F(this->m_shootx - this->m_viewportOffset.x, this->m_shooty - this->m_viewportOffset.y), 3.0, 3.0), this->m_debugBorderBrush.Get());
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
			(*mob)->m_spriteSourceRect
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