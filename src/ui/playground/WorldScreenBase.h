#pragma once

#include <thread>

#include "..\..\dx\DirectXHelper.h"
#include "..\..\dx\DeviceResources.h"
#include "..\..\io\BasicLoader.h"
#include "..\..\dx\BrushRegistry.h"
#include "..\..\dings\dings.h"
#include "..\WorldSheet.h"
#include "..\..\sound\Audio.h"

#define WORLDSHEET_NW this->m_Sheets[0][0]
#define WORLDSHEET_NE this->m_Sheets[1][0]
#define WORLDSHEET_SW this->m_Sheets[0][1]
#define WORLDSHEET_SE this->m_Sheets[1][1]

class WorldScreenBase
{
public:
	WorldScreenBase::WorldScreenBase();
	virtual WorldScreenBase::~WorldScreenBase();

	typedef std::map<int, DirectX::XMFLOAT2> TouchMap;

	/* graphical */
	virtual void Initialize(_In_ std::shared_ptr<Audio> audioEngine, _In_ std::shared_ptr<DX::DeviceResources>&	deviceResources);
	void CreateDeviceDependentResources();
	void ResetDirectXResources();
    void ReleaseDeviceDependentResources();
    void UpdateForWindowSizeChange();
	virtual void Update(float timeTotal, float timeDelta);
	virtual void Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition) { };

	/* logical */
	std::shared_ptr<Level> LoadLevel(Platform::String^ loadFromFile);
	std::shared_ptr<Level> LoadAndEnterLevel(Platform::String^ loadFromFile);
	void EnterLevel(std::shared_ptr<Level> level);
	virtual void SetControl(DirectX::XMFLOAT2 pointerPosition, TouchMap* touchMap, bool shiftKeyActive, bool left, bool right, bool up, bool down, float scrollDeltaX, float scrollDeltaY);
	virtual void SetControl(int detentCount, bool shiftKeyActive);

protected:
	const float										SCROLL_TRESHOLD_FACT = 0.61F;

	std::shared_ptr<DX::DeviceResources>			m_deviceResources;
	Microsoft::WRL::ComPtr<ID2D1Factory1>			m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device>				m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext>		m_d2dContext;
	std::shared_ptr<Audio>							m_audio;
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
	D2D1_RECT_F										m_viewportScrollTreshold;
	D2D1_POINT_2U									m_currentLevelEditorCell;
	bool											m_currentLevelEditorCellKnown;

	std::shared_ptr<Level>							m_currentLevel;
	bool											m_sheetHoveringSituationKnown;
	std::unique_ptr<WorldSheet>						m_Sheets[2][2] = { {nullptr, nullptr}, {nullptr, nullptr} };
	Microsoft::WRL::ComPtr<ID2D1Brush>				m_projectileBrush;
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID2D1Brush>				m_debugBorderBrush;
	Microsoft::WRL::ComPtr<ID2D1Brush>				m_debugTresholdBrush;
	unsigned										m_lastBlitSheetCount;
#endif
	D2D1_POINT_2F									m_pointerPosition;
	bool											m_keyShiftDown;
	TouchMap* 										m_touchMap;

	std::unique_ptr<WorldSheet>	InstantiateViewportSheet(unsigned sheetX, unsigned sheetY);
	void EvaluateSheetHoveringSituation();
	void InvalidateSheetHoveringSituation();
	void InvalidateLevelViewport();
	void ReflapBlitterSheets(D2D1_RECT_F viewPort, Facings towardsDirection);
	void RePopulateSheet(WorldSheet* targetSheet);
	bool PeekTouchdown();
	bool PopTouchdown();
	void ObliterateObject(D2D1_POINT_2U levelCoordinate);
	void ClumsyPackNeighborhoodOf(ClumsyPacking::NeighborConfiguration neighborHood, unsigned aroundLevelX, unsigned aroundLevelY, Layers inLayer, Dings::DingIDs dingID);
	void ClumsyPackNeighborhoodOf(unsigned aroundLevelX, unsigned aroundLevelY, Layers inLayer, Dings::DingIDs dingID);
	void RedrawSingleSquare(unsigned levelX, unsigned levelY, Layers inLayer);

private:
	void ComputeWorldSize();
	void ComputeWorldCenter();
	void ComputeViewportOffset();
	D2D1_POINT_2U GetViewportCenterInLevel();	
};
