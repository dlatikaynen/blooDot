#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\dx\BrushRegistry.h"
#include "..\algo\GameOfLife.h"
#include "..\algo\GameOfLifeSprinkler.h"

class LoadScreen
{
public:
    void Initialize(
        _In_ ID2D1Device*         d2dDevice,
        _In_ ID2D1DeviceContext*  d2dContext,
		_In_ std::shared_ptr<DX::DeviceResources>& deviceResources,
		_In_ IWICImagingFactory*  wicFactory
        );

	void CreateDeviceDependentResources();
	void ResetDirectXResources();
    void ReleaseDeviceDependentResources();

    void UpdateForWindowSizeChange();

	void Update(float timeTotal, float timeDelta);
	void Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition);

private:
	const float										GoLCellSizePerc = 0.0105f;
	const int										GoLSprinklerRadius = 4;
	const int										GoLCellSideLength = 10;

	std::shared_ptr<DX::DeviceResources>			m_deviceResources;
	
	Microsoft::WRL::ComPtr<ID2D1Factory1>           m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device>             m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext>      m_d2dContext;
    Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>  m_stateBlock;
	BrushRegistry									m_Brushes;

    Microsoft::WRL::ComPtr<IWICImagingFactory>      m_wicFactory;
    Microsoft::WRL::ComPtr<ID2D1Bitmap>             m_bitmap;

    D2D1_SIZE_F                                     m_imageSize;
    D2D1_SIZE_F                                     m_offset;
	D2D1_SIZE_F                                     m_moved;
	D2D1_SIZE_F                                     m_totalSize;
	bool											m_isResizing;

	GameOfLifeAnimation								m_GoLEngine;
	GameOfLifePlane*								m_GoL;
	GameOfLifeSprinkler*							m_Sprinkler;
};
