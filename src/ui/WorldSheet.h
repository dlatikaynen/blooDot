#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\io\BasicLoader.h"
#include "..\dx\BrushRegistry.h"
#include "..\dings\dings.h"

class WorldSheet
{
public:
	WorldSheet::WorldSheet();
	WorldSheet::~WorldSheet();

	D2D1_POINT_2U									CornerNW();
	D2D1_POINT_2U									CornerNE();
	D2D1_POINT_2U									CornerSW();
	D2D1_POINT_2U									CornerSE();
	D2D1_POINT_2U									GetCorner(Facings whichOne);

	void											Discard();

protected:
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_floor;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_walls;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_rooof;

	D2D1_SIZE_F                                     m_sizePixle;
	D2D1_SIZE_U										m_sizeUnits;
	D2D1_POINT_2U									m_leftUpperCornerInWorld;

private:
	bool											m_isPopulated;
};
