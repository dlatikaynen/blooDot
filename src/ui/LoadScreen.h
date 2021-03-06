#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\dx\BrushRegistry.h"
#include "..\sound\Audio.h"
#include "..\algo\GameOfLife.h"
#include "..\algo\GameOfLifeSprinkler.h"
#include "..\sound\SynthOfLife.h"
#include "..\sound\SynthSequence.h"

using namespace blooDot;

class LoadScreen
{
public:
	LoadScreen::LoadScreen();
    void Initialize(_In_ std::shared_ptr<DX::DeviceResources>& deviceResources, _In_ std::shared_ptr<Audio> audioEngine);
	void CreateDeviceDependentResources();
	void ResetDirectXResources();
    void ReleaseDeviceDependentResources();

    void UpdateForWindowSizeChange();
	void ResourceLoadingCompleted();

	void Update(float timeTotal, float timeDelta);
	void Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition);

private:
	inline int DipsToPixelsX(float dips, float dpix) const { return int(dips * dpix / 96.f + 0.5f);	};
	inline int DipsToPixelsY(float dips, float dpiy) const { return int(dips * dpiy / 96.f + 0.5f); };

	static constexpr float							GoLCellSizePerc = 0.0105f;
	static constexpr int							GoLSprinklerRadius = 4;
	static constexpr int							GoLCellSideLength = 10;

	std::shared_ptr<DX::DeviceResources>			m_deviceResources;	
	Microsoft::WRL::ComPtr<ID2D1Factory1>           m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device>             m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext>      m_d2dContext;
	std::shared_ptr<Audio>							m_audio;
    Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>  m_stateBlock;
	std::shared_ptr<BrushRegistry>					m_Brushes;

	D2D1_SIZE_F                                     m_moved;
	bool											m_isResizing;

	GameOfLifeAnimation								m_GoLEngine;
	GameOfLifeSprinkler*							m_Sprinkler;
	std::shared_ptr<SynthOfLife>					m_Synthesizer;
	std::shared_ptr<SynthSequence>					m_synthSequence;
	bool											m_resourceLoadingCompleted;
};
