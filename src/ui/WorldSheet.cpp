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
	auto levBounds = forLevel->GetRectBoundsUnits();
	auto sheetSize = forLevel->GetSheetSizeUnits();
	this->m_leftUpperCornerInWorld = D2D1::Point2U(amSheetX * sheetSize.width, amSheetY * sheetSize.height);
	this->m_sizeUnits = sheetSize;
	this->m_sizePixle = D2D1::SizeF(static_cast<float>(sheetSize.width) * blooDot::Consts::SQUARE_WIDTH, static_cast<float>(sheetSize.height) * blooDot::Consts::SQUARE_HEIGHT);
}

D2D1_POINT_2U WorldSheet::CornerNW()
{
	return this->m_leftUpperCornerInWorld;
}

D2D1_POINT_2U WorldSheet::CornerNE()
{
	return D2D1::Point2U(this->m_leftUpperCornerInWorld.x + this->m_sizeUnits.width, this->m_leftUpperCornerInWorld.y);
}

D2D1_POINT_2U WorldSheet::CornerSW()
{
	return D2D1::Point2U(this->m_leftUpperCornerInWorld.x, this->m_leftUpperCornerInWorld.y + this->m_sizeUnits.height);
}

D2D1_POINT_2U WorldSheet::CornerSE()
{
	return D2D1::Point2U(this->m_leftUpperCornerInWorld.x + this->m_sizeUnits.width, this->m_leftUpperCornerInWorld.y + this->m_sizeUnits.height);
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
		if (m_floor == nullptr)
		{
			DX::ThrowIfFailed(this->m_d2dContext->CreateCompatibleRenderTarget(this->m_sizePixle, &this->m_floor));
		}

		if (m_walls == nullptr)
		{
			DX::ThrowIfFailed(this->m_d2dContext->CreateCompatibleRenderTarget(this->m_sizePixle, &this->m_walls));
		}

		if (m_rooof == nullptr)
		{
			DX::ThrowIfFailed(this->m_d2dContext->CreateCompatibleRenderTarget(this->m_sizePixle, &this->m_rooof));
		}

		this->m_isPopulated = true;
	}
}

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

