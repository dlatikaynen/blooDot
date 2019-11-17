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
}

WorldSheet::~WorldSheet()
{
	this->Discard();
}

void WorldSheet::PrepareThyself(Level* forLevel, int amSheetX, int amSheetY)
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
	auto beganDrawWalls = false, beganDrawFloor = false, beganDrawRooof = false;

	if (!this->m_isPopulated)
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
		dingSheet->GetBitmap(&dingMap);
		for (auto y = 0; y < this->m_sizeUnits.height; ++y)
		{
			for (auto x = 0; x < this->m_sizeUnits.width; ++x)
			{
				auto worldX = x + this->m_NWcornerInWorldSquares.x;
				auto worldY = y + this->m_NWcornerInWorldSquares.y;
				auto objectX = this->m_tiedToLevel->GetObjectAt(worldX, worldY, false);
				if (objectX != nullptr)
				{
					auto layer = objectX->GetLayers();					
					auto dings = objectX->GetDing(layer);
					if (layer == Layers::Floor)
					{
						if (!beganDrawFloor)
						{
							this->m_floor->BeginDraw();
							beganDrawFloor = true;
						}

						this->PlacePrimitive(dingMap, this->m_floor, dings, objectX->PlacementFacing(), x, y);
					}
					else if (layer == Layers::Walls)
					{
						if (!beganDrawWalls)
						{
							this->m_walls->BeginDraw();
							beganDrawWalls = true;
						}

						this->PlacePrimitive(dingMap, this->m_walls, dings, objectX->PlacementFacing(), x, y);
					}
					else if (layer == Layers::Rooof)
					{
						if (!beganDrawRooof)
						{
							this->m_rooof->BeginDraw();
							beganDrawRooof = true;
						}

						this->PlacePrimitive(dingMap, this->m_rooof, dings, objectX->PlacementFacing(), x, y);
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

		this->m_isPopulated = true;
	}
}

void WorldSheet::RedrawSingleSquare(unsigned x, unsigned y)
{
	auto beganDrawWalls = false, beganDrawFloor = false, beganDrawRooof = false;
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
	dingSheet->GetBitmap(&dingMap);

	auto worldX = x + this->m_NWcornerInWorldSquares.x;
	auto worldY = y + this->m_NWcornerInWorldSquares.y;
	auto objectX = this->m_tiedToLevel->GetObjectAt(worldX, worldY, false);
	if (objectX == nullptr)
	{
		/* erase by transparenting everything on that layer */
		if (!beganDrawWalls)
		{
			this->m_walls->BeginDraw();
			beganDrawWalls = true;
		}

		this->EraseSquare(this->m_walls, x, y);
	}
	else
	{
		auto layer = objectX->GetLayers();
		auto dings = objectX->GetDing(layer);
		if (layer == Layers::Floor)
		{
			if (!beganDrawFloor)
			{
				this->m_floor->BeginDraw();
				beganDrawFloor = true;
			}

			this->PlacePrimitive(dingMap, this->m_floor, dings, objectX->PlacementFacing(), x, y);
		}
		else if (layer == Layers::Walls)
		{
			if (!beganDrawWalls)
			{
				this->m_walls->BeginDraw();
				beganDrawWalls = true;
			}

			this->PlacePrimitive(dingMap, this->m_walls, dings, objectX->PlacementFacing(), x, y);
		}
		else if (layer == Layers::Rooof)
		{
			if (!beganDrawRooof)
			{
				this->m_rooof->BeginDraw();
				beganDrawRooof = true;
			}

			this->PlacePrimitive(dingMap, this->m_rooof, dings, objectX->PlacementFacing(), x, y);
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
	auto sheetRect = D2D1::RectF(
		this->m_NWcornerInWorld.x, 
		this->m_NWcornerInWorld.y, 
		this->m_NWcornerInWorld.x + this->m_sizePixle.width, 
		this->m_NWcornerInWorld.y + this->m_sizePixle.height
	);

	/* gives bottom-left point of intersection rectangle */
	auto x5 = max(viewPort.left, sheetRect.left);
	auto y5 = min(viewPort.bottom, sheetRect.bottom);
	/* gives top-right point of intersection rectangle */
	auto x6 = min(viewPort.right, sheetRect.right);
	auto y6 = max(viewPort.top, sheetRect.top);
	auto overlapRect = D2D1::RectF(x5, y6, x6, y5);
	/* source is the overlap translated to co-ordinates locally relative to the sheet 
	 * destination is the overlap translated to co-ordinates locally relative to the viewport */
	this->SetBlittingArea(
		D2D1::RectF(
			overlapRect.left - sheetRect.left,
			overlapRect.top - sheetRect.top,
			overlapRect.right - sheetRect.left,
			overlapRect.bottom - sheetRect.top
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

void WorldSheet::Translate(D2D1_RECT_F viewPort, unsigned deltaX, unsigned deltaY)
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
	ID2D1Bitmap *bmp = NULL;
	
	//this->m_tiedToLevel->GetDingSheet()->GetBitmap(&bmp);
	//m_d2dContext->DrawBitmap(bmp, this->m_blitTo, 1.0F, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, this->m_blitFrom);
	//bmp->Release();

	this->m_floor->GetBitmap(&bmp);
	this->m_d2dContext->DrawBitmap(bmp, this->m_blitTo, 1.0F, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, this->m_blitFrom);
	bmp->Release();

	this->m_walls->GetBitmap(&bmp);
	this->m_d2dContext->DrawBitmap(bmp, this->m_blitTo, 1.0F, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, this->m_blitFrom);
	bmp->Release();

	this->m_rooof->GetBitmap(&bmp);
	this->m_d2dContext->DrawBitmap(bmp, this->m_blitTo, 1.0F, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, this->m_blitFrom);
	bmp->Release();
}

#ifdef _DEBUG
void WorldSheet::DebugDrawBorder(Microsoft::WRL::ComPtr<ID2D1Brush> brush)
{
	auto rrect = D2D1::RoundedRect(this->m_blitTo, 5.0F, 5.0F);
	this->m_d2dContext->DrawRoundedRectangle(&rrect, brush.Get());
}
#endif

void WorldSheet::Discard()
{
	m_isPopulated = false;

	if (m_floor != nullptr)
	{
		this->m_floor.Get()->Release();
		this->m_floor.Reset();
		this->m_floor = nullptr;
	}

	if (m_walls != nullptr)
	{
		this->m_walls.Get()->Release();
		this->m_walls.Reset();
		this->m_walls = nullptr;
	}

	if (m_rooof != nullptr)
	{
		this->m_rooof.Get()->Release();
		this->m_rooof.Reset();
		this->m_rooof = nullptr;
	}
}

