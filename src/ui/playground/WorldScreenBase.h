	#pragma once

#include "..\..\dx\DirectXHelper.h"
#include "..\..\dx\DeviceResources.h"
#include "..\..\io\BasicLoader.h"
#include "..\..\dx\BrushRegistry.h"
#include "..\..\dings\dings.h"
#include "..\WorldSheet.h"

class WorldScreenBase
{
public:
	WorldScreenBase::WorldScreenBase();
	virtual WorldScreenBase::~WorldScreenBase();

	typedef std::map<int, DirectX::XMFLOAT2> TouchMap;

	/* graphical */
	virtual void Initialize(_In_ std::shared_ptr<DX::DeviceResources>&	deviceResources);
	void CreateDeviceDependentResources();
	void ResetDirectXResources();
    void ReleaseDeviceDependentResources();
    void UpdateForWindowSizeChange();
	virtual void SetControl(DirectX::XMFLOAT2 pointerPosition, TouchMap* touchMap, bool shiftKeyActive, bool left, bool right, bool up, bool down, float scrollDeltaX, float scrollDeltaY);
	virtual void SetControl(int detentCount, bool shiftKeyActive);
	virtual void Update(float timeTotal, float timeDelta);
	virtual void Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition) { };

	/* logical */
	Level* LoadAndEnterLevel(Platform::String^ loadFromFile);
	void EnterLevel(Level* level);

protected:
	const float										SCROLL_TRESHOLD_FACT = 0.61F;

	std::shared_ptr<DX::DeviceResources>			m_deviceResources;
	Microsoft::WRL::ComPtr<ID2D1Factory1>			m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device>				m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext>		m_d2dContext;
    Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>	m_stateBlock;
	BrushRegistry									m_Brushes;
    Microsoft::WRL::ComPtr<IWICImagingFactory>		m_wicFactory;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>				m_notimeforcaution;
	D2D1_SIZE_F										m_viewportSize;
	D2D1_SIZE_U										m_viewportSizeSquares;
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
	unsigned										m_lastBlitSheetCount;
#endif
	D2D1_POINT_2F									m_pointerPosition;
	bool											m_keyShiftDown;
	TouchMap* 										m_touchMap;

	WorldSheet*	GetSheet(unsigned sheetX, unsigned sheetY);
	void EvaluateSheetHoveringSituation();
	void InvalidateSheetHoveringSituation();
	bool PeekTouchdown();
	bool PopTouchdown();

private:
	void ComputeWorldSize();
	void ComputeWorldCenter();
	void ComputeViewportOffset();
	D2D1_POINT_2U GetViewportCenterInLevel();	
};