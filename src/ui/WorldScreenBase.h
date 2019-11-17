#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\io\BasicLoader.h"
#include "..\dx\BrushRegistry.h"
#include "..\dings\dings.h"
#include "WorldSheet.h"

class WorldScreenBase
{
	typedef std::map<int, DirectX::XMFLOAT2> TouchMap;

public:
	WorldScreenBase::WorldScreenBase();
	virtual WorldScreenBase::~WorldScreenBase();
	
	/* graphical */
	void Initialize(_In_ std::shared_ptr<DX::DeviceResources>&	deviceResources);
	void CreateDeviceDependentResources();
	void ResetDirectXResources();
    void ReleaseDeviceDependentResources();
    void UpdateForWindowSizeChange();
	void SetControl(DirectX::XMFLOAT2 pointerPosition, TouchMap* touchMap, bool shiftKeyActive, bool left, bool right, bool up, bool down);
	virtual void Update(float timeTotal, float timeDelta);
	virtual void Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition);

	/* logical */
	void LoadAndEnterLevel(Platform::String^ loadFromFile);
	void EnterLevel(Level* level);

protected:
	const float										SCROLL_TRESHOLD_FACT = 0.61F;

	std::shared_ptr<DX::DeviceResources>			m_deviceResources;
	Microsoft::WRL::ComPtr<ID2D1Factory1>           m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device>             m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext>      m_d2dContext;
    Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>  m_stateBlock;
	BrushRegistry									m_Brushes;
    Microsoft::WRL::ComPtr<IWICImagingFactory>      m_wicFactory;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>             m_notimeforcaution;
	D2D1_SIZE_F										m_viewportSize;
	D2D1_SIZE_U                                     m_viewportSizeSquares;
	bool											m_isResizing;
	Facings											m_isMoving;
	unsigned										m_movingSpeedX;
	unsigned										m_movingSpeedY;

	D2D1_SIZE_F										m_worldSize;
	D2D1_POINT_2F									m_worldCenter;
	D2D1_POINT_2U									m_worldCenterSquares;
	D2D1_POINT_2F									m_viewportOffset;
	D2D1_POINT_2U									m_viewportOffsetSquares;
	D2D1_SIZE_F										m_viewportScrollTreshold;
	D2D1_POINT_2U									m_currentLevelEditorCell;
	bool											m_currentLevelEditorCellKnown;

	Level*											m_currentLevel;
	std::vector<WorldSheet*>						m_Sheets;
	bool											m_sheetHoveringSituationKnown;
	WorldSheet*										m_hoveringSheetNW;
	WorldSheet*										m_hoveringSheetNE;
	WorldSheet*										m_hoveringSheetSW;
	WorldSheet*										m_hoveringSheetSE;
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID2D1Brush>				m_debugBorderBrush;
#endif
	D2D1_POINT_2F									m_pointerPosition;
	TouchMap* 										m_touchMap;

	WorldSheet*	GetSheet(unsigned sheetX, unsigned sheetY);
	void EvaluateSheetHoveringSituation();
	void InvalidateSheetHoveringSituation();
	void PlacePrimitive(ID2D1Bitmap *dingSurface, Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> renderTarget, Dings* ding, Facings coalesce, int placementX, int placementY);
	bool PeekTouchdown();
	bool PopTouchdown();

private:
	void ComputeWorldSize();
	void ComputeWorldCenter();
	void ComputeViewportOffset();
	D2D1_POINT_2U GetViewportCenterInLevel();	
};
