#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\io\BasicLoader.h"
#include "..\dx\BrushRegistry.h"
#include "..\dings\dings.h"
#include "WorldSheet.h"

class WorldScreenBase
{
public:
	const float										SQUARE_WIDTH = 49.0;
	const float										SQUARE_HEIGHT = 49.0;

	WorldScreenBase::WorldScreenBase();
    void Initialize(_In_ std::shared_ptr<DX::DeviceResources>&	deviceResources);
	void CreateDeviceDependentResources();
	void ResetDirectXResources();
    void ReleaseDeviceDependentResources();
    void UpdateForWindowSizeChange();
	void SetControl(bool left, bool right, bool up, bool down);
	virtual void Update(float timeTotal, float timeDelta);
	virtual void Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition);

protected:
	const float										SCROLL_TRESHOLD_FACT = 0.61F;

	std::shared_ptr<DX::DeviceResources>			m_deviceResources;
	Microsoft::WRL::ComPtr<ID2D1Factory1>           m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device>             m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext>      m_d2dContext;
    Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>  m_stateBlock;
	BrushRegistry									m_Brushes;
    Microsoft::WRL::ComPtr<IWICImagingFactory>      m_wicFactory;
    Microsoft::WRL::ComPtr<ID2D1Bitmap>             m_background;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>             m_notimeforcaution;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_dingSheet;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_floor;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_walls;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_rooof;
	D2D1_SIZE_F                                     m_backgroundSize;
	D2D1_SIZE_U										m_viewportSize;
	D2D1_SIZE_U                                     m_viewportSizeSquares;
	bool											m_isResizing;
	Facings											m_isMoving;

	D2D1_SIZE_F										m_worldSize;
	D2D1_SIZE_U										m_worldSizeSquares;
	D2D1_POINT_2F									m_worldCenter;
	D2D1_POINT_2U									m_worldCenterSquares;
	D2D1_POINT_2F									m_viewportOffset;
	D2D1_POINT_2U									m_viewportOffsetSquares;
	D2D1_SIZE_F										m_viewportScrollTreshold;
	D2D1_POINT_2U									m_currentLevelEditorCell;

	WorldSheet*										m_Sheets;

	void PlacePrimitive(ID2D1Bitmap *dingSurface, Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> renderTarget, Dings* ding, Facings coalesce, int placementX, int placementY);

private:
	void ComputeWorldSize();
	void ComputeWorldCenter();
	void ComputeViewportOffset();
};
