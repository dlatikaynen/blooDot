#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\algo\GameOfLifeCell.h"

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
	std::shared_ptr<DX::DeviceResources>			m_deviceResources;
	
	Microsoft::WRL::ComPtr<ID2D1Factory1>           m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device>             m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext>      m_d2dContext;
    Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>  m_stateBlock;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_GoLBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_GoLBrushRaindrop;

    Microsoft::WRL::ComPtr<IWICImagingFactory>      m_wicFactory;
    Microsoft::WRL::ComPtr<ID2D1Bitmap>             m_bitmap;

    D2D1_SIZE_F                                     m_imageSize;
    D2D1_SIZE_F                                     m_offset;
	D2D1_SIZE_F                                     m_moved;
	D2D1_SIZE_F                                     m_totalSize;

	GameOfLifeCell m_GoL[100][60];
	GameOfLifeCell m_GoL2[100][60];
	bool LoadScreen::NeighborN(int i, int j);
	bool LoadScreen::NeighborS(int i, int j);
	bool LoadScreen::NeighborE(int i, int j);
	bool LoadScreen::NeighborNE(int i, int j);
	bool LoadScreen::NeighborSE(int i, int j);
	bool LoadScreen::NeighborW(int i, int j);
	bool LoadScreen::NeighborNW(int i, int j);
	bool LoadScreen::NeighborSW(int i, int j);
	int LoadScreen::NeighborsAlive(int i, int j);
	int LoadScreen::IndexLeft(int i);
	int LoadScreen::IndexRight(int i);
	int LoadScreen::IndexUp(int j);
	int LoadScreen::IndexDown(int j);
	void DrawCell(int x, int y);
};
