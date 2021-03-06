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
	WorldSheet::WorldSheet(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> brushRegistry);
	WorldSheet::~WorldSheet();

	void											PrepareThyself(std::shared_ptr<Level> forLevel, int amSheetX, int amSheetY);

	D2D1_RECT_F										PhysicalPosition;
	D2D1_POINT_2U									TheSheetIAm();
	float											PhysicalWidth();
	float											PhysicalHeight();

	bool											IsPopulated();
	void											ForceRePopulate();
	void											Populate();
	void											RedrawSingleSquare(unsigned relativeX, unsigned relativeY, Layers inLayer);
	void											ComputeViewportOverlap(D2D1_RECT_F viewPort);
	void											SetBlittingArea(D2D1_RECT_F blitFrom, D2D1_RECT_F blitTo);
	void											Translate(D2D1_RECT_F viewPort, float deltaX, float deltaY);
	void											UpdatePositionInLevel(int unitsDeltaX, int unitsDeltaY);
	void											BlitToViewport();
	void											BlitToViewportFloor();
	void											BlitToViewportWallsRooof();
#ifdef _DEBUG
	void											DebugDrawBorder(Microsoft::WRL::ComPtr<ID2D1Brush> brush);
#endif
	void											Discard();

protected:
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_floor;
	ID2D1Bitmap*									m_floorBitmap;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_walls;
	ID2D1Bitmap*									m_wallsBitmap;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_rooof;
	ID2D1Bitmap*									m_rooofBitmap;

	D2D1_SIZE_U										m_sizeUnits;
	D2D1_SIZE_F										m_sizePixle;
	D2D1_POINT_2F									m_NWcornerInWorld;
	D2D1_POINT_2U									m_NWcornerInWorldSquares;

private:
	bool											m_isPopulated;
	std::shared_ptr<Level>							m_tiedToLevel;
	D2D1_POINT_2U									m_theSheetIAm;
	std::shared_ptr<DX::DeviceResources>			m_deviceResources;
	Microsoft::WRL::ComPtr<ID2D1Factory1>           m_d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device>             m_d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext>      m_d2dContext;
	std::shared_ptr<BrushRegistry>					m_brushes;
	D2D1_RECT_F										m_blitFrom;
	D2D1_RECT_F										m_blitTo;

	D2D1_RECT_F GetFloorBounds();
	void PlacePrimitive(ID2D1Bitmap *dingSurface, Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> renderTarget, std::shared_ptr<Dings> ding, Facings coalesce, int placementX, int placementY);
	void EraseSquare(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> renderTarget, int placementX, int placementY);
	void FreeBitmaps();

	template <class T> void SafeRelease(T **ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}
};
