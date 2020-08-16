#include "..\PreCompiledHeaders.h"
#include "WorldSheet.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;

WorldSheet::WorldSheet(std::shared_ptr<DX::DeviceResources> deviceResources)
{
	this->m_deviceResources = deviceResources;
	this->m_floorBitmap = nullptr;
	this->m_wallsBitmap = nullptr;
	this->m_rooofBitmap = nullptr;
	this->m_isPopulated = false;
}

WorldSheet::~WorldSheet()
{
	this->Discard();
}

void WorldSheet::PrepareThyself(std::shared_ptr<Level> forLevel, int amSheetX, int amSheetY)
{
	this->m_d2dContext = this->m_deviceResources->GetD2DDeviceContext();
	this->m_d2dFactory = this->m_deviceResources->GetD2DFactory();
	this->m_d2dDevice = this->m_deviceResources->GetD2DDevice();
	this->m_tiedToLevel = forLevel;
	this->m_theSheetIAm = D2D1::Point2U(amSheetX, amSheetY);
	auto levBounds = forLevel->GetRectBoundsUnits();
	auto sheetSize = forLevel->GetSheetSizeUnits();
	this->m_NWcornerInWorldSquares = D2D1::Point2U(amSheetX * sheetSize.width, amSheetY * sheetSize.height);
	this->m_NWcornerInWorld = D2D1::Point2F(static_cast<float>(amSheetX * sheetSize.width) * blooDot::Consts::SQUARE_WIDTH, static_cast<float>(amSheetY * sheetSize.height) * blooDot::Consts::SQUARE_WIDTH);
	this->m_sizeUnits = sheetSize;
	this->m_sizePixle = D2D1::SizeF(static_cast<float>(sheetSize.width) * blooDot::Consts::SQUARE_WIDTH, static_cast<float>(sheetSize.height) * blooDot::Consts::SQUARE_HEIGHT);
}

D2D1_POINT_2U WorldSheet::TheSheetIAm()
{
	return this->m_theSheetIAm;
}

D2D1_POINT_2U WorldSheet::CornerNW()
{
	return this->m_NWcornerInWorldSquares;
}

D2D1_POINT_2U WorldSheet::CornerNE()
{
	return D2D1::Point2U(this->m_NWcornerInWorldSquares.x + this->m_sizeUnits.width, this->m_NWcornerInWorldSquares.y);
}

D2D1_POINT_2U WorldSheet::CornerSW()
{
	return D2D1::Point2U(this->m_NWcornerInWorldSquares.x, this->m_NWcornerInWorldSquares.y + this->m_sizeUnits.height);
}

D2D1_POINT_2U WorldSheet::CornerSE()
{
	return D2D1::Point2U(this->m_NWcornerInWorldSquares.x + this->m_sizeUnits.width, this->m_NWcornerInWorldSquares.y + this->m_sizeUnits.height);
}

D2D1_POINT_2U WorldSheet::GetCorner(Facings whichOne)
{
	switch (whichOne) 
	{
		case Facings::NW: return this->CornerNW();
		case Facings::NE: return this->CornerNE();
		case Facings::SW: return this->CornerSW();
		case Facings::SE: return this->CornerSE();
	}

	return D2D1_POINT_2U();
}

bool WorldSheet::IsPopulated()
{
	return this->m_isPopulated;
}

// Draw the Ding Wang Tang onto the sheet
void WorldSheet::Populate()
{
	if (!this->m_isPopulated)
	{
		auto beganDrawWalls = false, beganDrawFloor = false, beganDrawRooof = false;
		this->FreeBitmaps();
		if (m_floor == nullptr)
		{
			DX::ThrowIfFailed(this->m_d2dContext->CreateCompatibleRenderTarget(this->m_sizePixle, &this->m_floor));
			if (!beganDrawFloor)
			{
				this->m_floor->BeginDraw();
				beganDrawFloor = true;
				auto floorBackground = this->m_tiedToLevel->GetFloorBackground().Get();
				this->m_floor->DrawBitmap(floorBackground, this->GetFloorBounds(), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, this->GetFloorBounds());
			}
		}

		if (m_walls == nullptr)
		{
			DX::ThrowIfFailed(this->m_d2dContext->CreateCompatibleRenderTarget(this->m_sizePixle, &this->m_walls));
		}

		if (m_rooof == nullptr)
		{
			DX::ThrowIfFailed(this->m_d2dContext->CreateCompatibleRenderTarget(this->m_sizePixle, &this->m_rooof));
		}

		auto dingSheet = this->m_tiedToLevel->GetDingSheet();
		ID2D1Bitmap *dingMap = NULL;
		Dings* dings;
		dingSheet->GetBitmap(&dingMap);
		for (unsigned y = 0; y < this->m_sizeUnits.height; ++y)
		{
			for (unsigned x = 0; x < this->m_sizeUnits.width; ++x)
			{
				auto worldX = x + this->m_NWcornerInWorldSquares.x;
				auto worldY = y + this->m_NWcornerInWorldSquares.y;
				auto objectX = this->m_tiedToLevel->GetObjectAt(worldX, worldY, false);
				if (objectX != nullptr)
				{
					auto layer = objectX->GetLayers();
					if ((layer & Layers::Floor) == Layers::Floor)
					{
						if (!beganDrawFloor)
						{
							this->m_floor->BeginDraw();
							beganDrawFloor = true;
						}

						dings = objectX->GetDing(Layers::Floor);
						this->PlacePrimitive(dingMap, this->m_floor, dings, objectX->PlacementFacing(::Floor), x, y);
					}
					
					if ((layer & Layers::Walls) == Layers::Walls)
					{
						if (!beganDrawWalls)
						{
							this->m_walls->BeginDraw();
							beganDrawWalls = true;
						}

						dings = objectX->GetDing(Layers::Walls);
						this->PlacePrimitive(dingMap, this->m_walls, dings, objectX->PlacementFacing(::Walls), x, y);
					}
					
					if ((layer & Layers::Rooof) == Layers::Rooof)
					{
						if (!beganDrawRooof)
						{
							this->m_rooof->BeginDraw();
							beganDrawRooof = true;
						}

						dings = objectX->GetDing(Layers::Rooof);
						this->PlacePrimitive(dingMap, this->m_rooof, dings, objectX->PlacementFacing(::Rooof), x, y);
					}
				}
			}
		}

		dingMap->Release();
		if (beganDrawFloor)
		{
			DX::ThrowIfFailed(this->m_floor->EndDraw());
		}

		if (beganDrawWalls)
		{
			DX::ThrowIfFailed(this->m_walls->EndDraw());

		}

		if (beganDrawRooof)
		{
			DX::ThrowIfFailed(this->m_rooof->EndDraw());
		}

		this->m_floor.Get()->GetBitmap(&this->m_floorBitmap);
		this->m_walls.Get()->GetBitmap(&this->m_wallsBitmap);
		this->m_rooof.Get()->GetBitmap(&this->m_rooofBitmap);
		this->m_isPopulated = true;
	}
}

void WorldSheet::RedrawSingleSquare(unsigned x, unsigned y, Layers inLayer)
{
	auto beganDrawWalls = false, beganDrawFloor = false, beganDrawRooof = false;
	auto doFloor = inLayer & Layers::Floor, doWalls = inLayer & Layers::Walls, doRooof = inLayer & Layers::Rooof;
	Dings* dings = nullptr;

	if (doFloor)
	{
		if (m_floor == nullptr)
		{
			DX::ThrowIfFailed(this->m_d2dContext->CreateCompatibleRenderTarget(this->m_sizePixle, &this->m_floor));
			if (!beganDrawFloor)
			{
				this->m_floor->BeginDraw();
				beganDrawFloor = true;
				auto floorBackground = this->m_tiedToLevel->GetFloorBackground().Get();
				this->m_floor->DrawBitmap(floorBackground, this->GetFloorBounds(), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, this->GetFloorBounds());
			}
		}
	}

	if (doWalls && m_walls == nullptr)
	{
		DX::ThrowIfFailed(this->m_d2dContext->CreateCompatibleRenderTarget(this->m_sizePixle, &this->m_walls));
	}

	if (doRooof && m_rooof == nullptr)
	{
		DX::ThrowIfFailed(this->m_d2dContext->CreateCompatibleRenderTarget(this->m_sizePixle, &this->m_rooof));
	}

	auto dingSheet = this->m_tiedToLevel->GetDingSheet();
	ID2D1Bitmap *dingMap = NULL;
	dingSheet->GetBitmap(&dingMap);
	auto worldX = x + this->m_NWcornerInWorldSquares.x;
	auto worldY = y + this->m_NWcornerInWorldSquares.y;
	auto objectX = this->m_tiedToLevel->GetObjectAt(worldX, worldY, false);
	if (objectX == nullptr)
	{
		/* erase by transparenting everything on that layer */
		if (doFloor)
		{
			if (!beganDrawFloor)
			{
				this->m_floor->BeginDraw();
				beganDrawFloor = true;
			}

			auto floorBackground = this->m_tiedToLevel->GetFloorBackground().Get();
			D2D1_RECT_F replacementRect = D2D1::RectF(x * 49.0f, y * 49.0f, x * 49.0f + 49.0f, y * 49.0f + 49.0f);
			this->m_floor->DrawBitmap(floorBackground, replacementRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, replacementRect);
		}

		if (doWalls)
		{
			if (!beganDrawWalls)
			{
				this->m_walls->BeginDraw();
				beganDrawWalls = true;
			}

			this->EraseSquare(this->m_walls, x, y);
		}

		if (doRooof)
		{		
			if (!beganDrawRooof)
			{
				this->m_rooof->BeginDraw();
				beganDrawRooof = true;
			}

			this->EraseSquare(this->m_rooof, x, y);
		}
	}
	else
	{
		auto layers = objectX->GetLayers();
		if (doFloor)
		{
			if (!beganDrawFloor)
			{
				this->m_floor->BeginDraw();
				beganDrawFloor = true;
			}

			dings = objectX->GetDing(Layers::Floor);
			if (dings == nullptr)
			{
				auto floorBackground = this->m_tiedToLevel->GetFloorBackground().Get();
				D2D1_RECT_F replacementRect = D2D1::RectF(x * 49.0f, y * 49.0f, x * 49.0f + 49.0f, y * 49.0f + 49.0f);
				this->m_floor->DrawBitmap(floorBackground, replacementRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, replacementRect);
			}
			else			
			{
				this->PlacePrimitive(dingMap, this->m_floor, dings, objectX->PlacementFacing(::Floor), x, y);
			}
		}
		
		if (doWalls)
		{
			if (!beganDrawWalls)
			{
				this->m_walls->BeginDraw();
				beganDrawWalls = true;
				this->EraseSquare(this->m_walls, x, y);
			}

			dings = objectX->GetDing(Layers::Walls);
			if (dings != nullptr)
			{
				this->PlacePrimitive(dingMap, this->m_walls, dings, objectX->PlacementFacing(::Walls), x, y);
			}
		}
		
		if (doRooof)
		{
			if (!beganDrawRooof)
			{
				this->m_rooof->BeginDraw();
				beganDrawRooof = true;
				this->EraseSquare(this->m_rooof, x, y);
			}

			dings = objectX->GetDing(Layers::Rooof);
			if (dings != nullptr)
			{
				this->PlacePrimitive(dingMap, this->m_rooof, dings, objectX->PlacementFacing(::Rooof), x, y);
			}			
		}
	}

	dingMap->Release();
	if (beganDrawFloor)
	{
		DX::ThrowIfFailed(this->m_floor->EndDraw());
	}

	if (beganDrawWalls)
	{
		DX::ThrowIfFailed(this->m_walls->EndDraw());
	}

	if (beganDrawRooof)
	{
		DX::ThrowIfFailed(this->m_rooof->EndDraw());
	}
}

float WorldSheet::PhysicalWidth()
{
	return this->m_sizePixle.width;
}

float WorldSheet::PhysicalHeight()
{
	return this->m_sizePixle.height;
}

D2D1_RECT_F WorldSheet::GetFloorBounds()
{
	auto size = this->m_floor->GetSize();
	return D2D1::RectF(
		0.0F,
		0.0F,
		size.width,
		size.height
	);
}

void WorldSheet::PlacePrimitive(ID2D1Bitmap *dingSurface, Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> renderTarget, Dings* ding, Facings coalesce, int placementX, int placementY)
{
	auto dingOnSheet = ding->GetSheetPlacement(coalesce);
	D2D1_RECT_F dingRect = D2D1::RectF(dingOnSheet.x * 49.0f, dingOnSheet.y * 49.0f, dingOnSheet.x * 49.0f + 49.0f, dingOnSheet.y * 49.0f + 49.0f);
	D2D1_RECT_F placementRect = D2D1::RectF(placementX * 49.0f, placementY * 49.0f, placementX * 49.0f + 49.0f, placementY * 49.0f + 49.0f);
	renderTarget->DrawBitmap(dingSurface, placementRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, dingRect);	
}

void WorldSheet::EraseSquare(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> renderTarget, int placementX, int placementY)
{
	D2D1_RECT_F placementRect = D2D1::RectF(placementX * 49.0f, placementY * 49.0f, placementX * 49.0f + 49.0f, placementY * 49.0f + 49.0f);
	renderTarget->PushAxisAlignedClip(placementRect, D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	renderTarget->Clear();
	renderTarget->PopAxisAlignedClip();
}

void WorldSheet::ComputeViewportOverlap(D2D1_RECT_F viewPort)
{
	this->PhysicalPosition.left = this->m_NWcornerInWorld.x;
	this->PhysicalPosition.top = this->m_NWcornerInWorld.y;
	this->PhysicalPosition.right = this->m_NWcornerInWorld.x + this->m_sizePixle.width;
	this->PhysicalPosition.bottom = this->m_NWcornerInWorld.y + this->m_sizePixle.height;

	/* gives bottom-left point of intersection rectangle */
	auto x5 = max(viewPort.left, this->PhysicalPosition.left);
	auto y5 = min(viewPort.bottom, this->PhysicalPosition.bottom);
	/* gives top-right point of intersection rectangle */
	auto x6 = min(viewPort.right, this->PhysicalPosition.right);
	auto y6 = max(viewPort.top, this->PhysicalPosition.top);
	auto overlapRect = D2D1::RectF(x5, y6, x6, y5);
	/* source is the overlap translated to co-ordinates locally relative to the sheet 
	 * destination is the overlap translated to co-ordinates locally relative to the viewport */
	this->SetBlittingArea(
		D2D1::RectF(
			overlapRect.left - this->PhysicalPosition.left,
			overlapRect.top - this->PhysicalPosition.top,
			overlapRect.right - this->PhysicalPosition.left,
			overlapRect.bottom - this->PhysicalPosition.top
		),
		D2D1::RectF(
			overlapRect.left - viewPort.left,
			overlapRect.top - viewPort.top,
			overlapRect.right - viewPort.left,
			overlapRect.bottom - viewPort.top
		)
	);
}

void WorldSheet::SetBlittingArea(D2D1_RECT_F blitFrom, D2D1_RECT_F blitTo)
{
	this->m_blitFrom = blitFrom;
	this->m_blitTo = blitTo;
}

void WorldSheet::Translate(D2D1_RECT_F viewPort, float deltaX, float deltaY)
{
	if (deltaX != 0)
	{
		this->m_NWcornerInWorld.x -= deltaX;
	}

	if (deltaY != 0)
	{
		this->m_NWcornerInWorld.y -= deltaY;
	}
	
	this->ComputeViewportOverlap(viewPort);
}

void WorldSheet::BlitToViewport()
{	
	this->m_d2dContext->DrawBitmap(this->m_floorBitmap, this->m_blitTo, 1.0F, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, this->m_blitFrom);
	this->m_d2dContext->DrawBitmap(this->m_wallsBitmap, this->m_blitTo, 1.0F, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, this->m_blitFrom);
	this->m_d2dContext->DrawBitmap(this->m_rooofBitmap, this->m_blitTo, 1.0F, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, this->m_blitFrom);
}

void WorldSheet::BlitToViewportFloor()
{
	this->m_d2dContext->DrawBitmap(this->m_floorBitmap, this->m_blitTo, 1.0F, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, this->m_blitFrom);
}

void WorldSheet::BlitToViewportWallsRooof()
{
	this->m_d2dContext->DrawBitmap(this->m_wallsBitmap, this->m_blitTo, 1.0F, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, this->m_blitFrom);
	this->m_d2dContext->DrawBitmap(this->m_rooofBitmap, this->m_blitTo, 1.0F, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, this->m_blitFrom);
}

#ifdef _DEBUG
void WorldSheet::DebugDrawBorder(Microsoft::WRL::ComPtr<ID2D1Brush> brush)
{
	auto rrect = D2D1::RoundedRect(this->m_blitTo, 5.0F, 5.0F);
	this->m_d2dContext->DrawRoundedRectangle(&rrect, brush.Get());
}
#endif

void WorldSheet::FreeBitmaps()
{
	if (!(this->m_floorBitmap == nullptr))
	{
		this->SafeRelease(&this->m_floorBitmap);
	}

	if (!(this->m_floorBitmap == nullptr))
	{
		this->SafeRelease(&this->m_wallsBitmap);	
	}

	if (!(this->m_floorBitmap == nullptr))
	{
		this->SafeRelease(&this->m_rooofBitmap);
	}
}

void WorldSheet::Discard()
{
	this->FreeBitmaps();
	this->m_isPopulated = false;
	if (this->m_floor != nullptr)
	{
		this->m_floor.Reset();
		this->m_floor = nullptr;
	}

	if (this->m_walls != nullptr)
	{
		this->m_walls.Reset();
		this->m_walls = nullptr;
	}

	if (this->m_rooof != nullptr)
	{
		this->m_rooof.Reset();
		this->m_rooof = nullptr;
	}
}

