#include "..\..\PreCompiledHeaders.h"
#include "..\UserInterface.h"
#include "..\..\dx\DirectXHelper.h"

WorldScreen::WorldScreen() : WorldScreenBase()
{

}

WorldScreen::~WorldScreen()
{	

}

void WorldScreen::Initialize(_In_ std::shared_ptr<Audio> audioEngine, _In_ std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	WorldScreenBase::Initialize(audioEngine, deviceResources);
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
	bool scrollTresholdExceeded = false;
	bool scrollTresholdHandledX = false;
	bool scrollTresholdHandledY = false;
	float scrollTresholdExcessX = 0.0f;
	float scrollTresholdExcessY = 0.0f;
	for (auto mob = this->m_playerData.begin(); mob != this->m_playerData.end(); ++mob)
	{		
		auto Player = (*mob);
		Player->Update();
		if ((Player->Position.left - this->m_viewportOffset.x) < this->m_viewportScrollTreshold.left)
		{
			scrollTresholdExceeded = true;
			scrollTresholdExcessX = -(this->m_viewportScrollTreshold.left - (Player->Position.left - this->m_viewportOffset.x));
			scrollTresholdHandledX = true;
		}

		if ((Player->Position.top - this->m_viewportOffset.y) < this->m_viewportScrollTreshold.top)
		{
			scrollTresholdExceeded = true;
			scrollTresholdExcessY = -(this->m_viewportScrollTreshold.top - (Player->Position.top - this->m_viewportOffset.y));
			scrollTresholdHandledY = true;
		}

		if (!scrollTresholdHandledX && (Player->Position.right - this->m_viewportOffset.x) > this->m_viewportScrollTreshold.right)
		{
			scrollTresholdExceeded = true;
			scrollTresholdExcessX = Player->Position.right - this->m_viewportOffset.x - this->m_viewportScrollTreshold.right;
			scrollTresholdHandledX = true;
		}

		if (!scrollTresholdHandledY && (Player->Position.bottom - this->m_viewportOffset.y) > this->m_viewportScrollTreshold.bottom)
		{
			scrollTresholdExceeded = true;
			scrollTresholdExcessY = Player->Position.bottom - this->m_viewportOffset.y - this->m_viewportScrollTreshold.bottom;
			scrollTresholdHandledY = true;
		}
	}

	/* set the viewport in motion, if a protagonist mob crosses the scroll treshold */
	if (scrollTresholdExceeded)
	{
		this->m_viewportOffset = D2D1::Point2F(this->m_viewportOffset.x + static_cast<float>(scrollTresholdExcessX), this->m_viewportOffset.y + static_cast<float>(scrollTresholdExcessY));
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

	/* slock'em */
	if (this->m_shooting)
	{
		this->UpdateParticles(timeTotal, timeDelta);
	}

	/* scroll the viewport through the world, essentially */
	WorldScreenBase::Update(timeTotal, timeDelta);
}

void WorldScreen::UpdateParticles(float timeTotal, float timeDelta)
{
	auto deltaX = this->m_shoot_directionX;
	auto deltaY = this->m_shoot_directionY;
	if (this->m_blockstravelled++ > (196))
	{
		this->m_shooting = false;
	}
	else
	{
		auto squareX = this->m_shootx / blooDot::Consts::SQUARE_WIDTH;
		auto squareY = this->m_shooty / blooDot::Consts::SQUARE_HEIGHT;
		auto newPosition = D2D1::RectF(
			this->m_shootx - blooDot::Consts::LASERBULLET_RADIUS + deltaX, 
			this->m_shooty - blooDot::Consts::LASERBULLET_RADIUS + deltaY, 
			this->m_shootx + blooDot::Consts::LASERBULLET_RADIUS + deltaX, 
			this->m_shooty + blooDot::Consts::LASERBULLET_RADIUS + deltaY
		);

		if (deltaX < 0.0F)
		{
			/* hit something int the west? */
			auto westCenterTerrain = this->m_currentLevel->GetObjectAt(squareX - 1, squareY, false);
			if (westCenterTerrain != nullptr && westCenterTerrain->m_BehaviorsWalls != ObjectBehaviors::Boring)
			{
				D2D1_RECT_F westCenterBoundingBox;
				westCenterTerrain->GetBoundingBox(&westCenterBoundingBox);
				if (westCenterBoundingBox.right > newPosition.left)
				{
					auto originalWidth = newPosition.right - newPosition.left;
					auto wouldPenetrate = westCenterBoundingBox.right - newPosition.left;
					newPosition.left = westCenterBoundingBox.right + 1.0F;
					newPosition.right = newPosition.left + originalWidth;
					if (this->OnHit(westCenterTerrain))
					{
						this->m_shooting = false;
					}
				}
			}
		}

		if (deltaX > 0.0F)
		{
			/* hit something in the east?
			 * we browse the environment in the order of most likely interaction,
			 * so in this case this will be next-to-right first */
			auto eastCenterTerrain = this->m_currentLevel->GetObjectAt(squareX + 1, squareY, false);
			if (eastCenterTerrain != nullptr && eastCenterTerrain->m_BehaviorsWalls != ObjectBehaviors::Boring)
			{
				D2D1_RECT_F eastCenterBoundingBox;
				eastCenterTerrain->GetBoundingBox(&eastCenterBoundingBox);
				if (eastCenterBoundingBox.left < newPosition.right)
				{
					auto originalWidth = newPosition.right - newPosition.left;
					auto wouldPenetrate = newPosition.right - eastCenterBoundingBox.left;
					newPosition.right = eastCenterBoundingBox.left - 1.0F;
					newPosition.left = newPosition.right - originalWidth;
					if (this->OnHit(eastCenterTerrain))
					{
						this->m_shooting = false;
					}
				}
			}
		}

		if (deltaY < 0.0F)
		{
			/* hit something to the north? */
			auto northCenterTerrain = this->m_currentLevel->GetObjectAt(squareX, squareY - 1, false);
			if (northCenterTerrain != nullptr && northCenterTerrain->m_BehaviorsWalls != ObjectBehaviors::Boring)
			{
				D2D1_RECT_F northCenterBoundingBox;
				northCenterTerrain->GetBoundingBox(&northCenterBoundingBox);
				if (northCenterBoundingBox.bottom > newPosition.top)
				{
					auto originalHeight = newPosition.bottom - newPosition.top;
					auto wouldPenetrate = northCenterBoundingBox.bottom - newPosition.top;
					newPosition.top = northCenterBoundingBox.bottom + 1.0F;
					newPosition.bottom = newPosition.top + originalHeight;
					if (this->OnHit(northCenterTerrain))
					{
						this->m_shooting = false;
					}
				}
			}
		}

		if (deltaY > 0.0F)
		{
			/* hit something in the south? */
			auto southCenterTerrain = this->m_currentLevel->GetObjectAt(squareX, squareY + 1, false);
			if (southCenterTerrain != nullptr && southCenterTerrain->m_BehaviorsWalls != ObjectBehaviors::Boring)
			{
				D2D1_RECT_F southCenterBoundingBox;
				southCenterTerrain->GetBoundingBox(&southCenterBoundingBox);
				if (southCenterBoundingBox.top < newPosition.bottom)
				{
					auto originalHeight = newPosition.bottom - newPosition.top;
					auto wouldPenetrate = newPosition.bottom - southCenterBoundingBox.top;
					newPosition.bottom = southCenterBoundingBox.top - 1.0F;
					newPosition.top = newPosition.bottom - originalHeight;
					if (this->OnHit(southCenterTerrain))
					{
						this->m_shooting = false;
					}
				}
			}
		}

		this->m_shootx += deltaX;
		this->m_shooty += deltaY;
	}
}

bool WorldScreen::OnHit(Object* hitTarget)
{	
	auto underlyingDing = hitTarget->GetDing(Layers::Walls);
	if (underlyingDing == nullptr)
	{
		return false;
	}
	else
	{
		auto dingProps = underlyingDing->GetInherentBehaviors();
		if (dingProps & ObjectBehaviors::Shootable)
		{
			this->m_audio->PlaySoundEffect(SoundEvent::HitCrumble);
			this->ObliterateObject(hitTarget->PositionSquare);
		}
		else if (dingProps & ObjectBehaviors::Solid)
		{
			this->m_audio->PlaySoundEffect(SoundEvent::ProjectileDecay);
		}

		/* return value: true to decay the projectile */
		return !(dingProps & ObjectBehaviors::Immersible);
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

	/* most particles are below the player's head */
	if (this->m_shooting)
	{
		this->m_d2dContext->FillEllipse(D2D1::Ellipse(D2D1::Point2F(this->m_shootx - this->m_viewportOffset.x, this->m_shooty - this->m_viewportOffset.y), 3.0, 3.0), this->m_projectileBrush.Get());
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

#if _DEBUG
	this->m_d2dContext->DrawRectangle(this->m_viewportScrollTreshold, this->m_debugTresholdBrush.Get(), 0.75f);
#endif

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