#include "..\PreCompiledHeaders.h"
#include "WorldSheet.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;

WorldSheet::WorldSheet()
{
}

WorldSheet::~WorldSheet()
{
	this->Discard();
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

