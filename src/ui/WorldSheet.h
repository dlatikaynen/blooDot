#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\io\BasicLoader.h"
#include "..\dx\BrushRegistry.h"
#include "..\dings\dings.h"
#include "..\world\Level.h"
//#include "..\ui\WorldScreenBase.h"

class WorldSheet
{
public:
	WorldSheet::WorldSheet(std::shared_ptr<DX::DeviceResources> deviceResources);
	WorldSheet::~WorldSheet();

	void											PrepareThyself(Level* forLevel, int amSheetX, int amSheetY);

	D2D1_POINT_2U									CornerNW();
	D2D1_POINT_2U									CornerNE();
	D2D1_POINT_2U									CornerSW();
	D2D1_POINT_2U									CornerSE();
	D2D1_POINT_2U									GetCorner(Facings whichOne);

	void											Populate();
	bool											IsPopulated();
	void											Discard();

protected:
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_floor;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_walls;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_rooof;

	D2D1_SIZE_U										m_sizeUnits;
	D2D1_SIZE_F										m_sizePixle;
	D2D1_POINT_2U									m_leftUpperCornerInWorld;

private:
	bool											m_isPopulated;
	Level*											m_tiedToLevel;
	std::shared_ptr<DX::DeviceResources>			m_deviceResources;
	Microsoft::WRL::ComPtr<ID2D1Factory1>           m_d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device>             m_d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext>      m_d2dContext;

	void PlacePrimitive(ID2D1Bitmap *dingSurface, Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> renderTarget, Dings* ding, Facings coalesce, int placementX, int placementY);
};
