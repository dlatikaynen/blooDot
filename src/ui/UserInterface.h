#pragma once

#include "..\dx\BrushRegistry.h"
#include "TextStyle.h"
#include "ElementBase.h"
#include "TextElement.h"
#include "CountdownTimer.h"
#include "StopwatchTimer.h"
#include "NerdStatsDisplay.h"
#include "TextButton.h"
#include "HighScoreTable.h"
#include "playground\WorldScreenBase.h"
#include "playground\WorldScreen.h"
#include "playground\LevelEditor.h"
#include "LevelEditorHUD.h"
#include "DialogOverlay.h"
#include "ControllerSetup.h"

class UserInterface
{
public:
    static UserInterface& GetInstance() { return m_instance; }

    static Microsoft::WRL::ComPtr<ID2D1Factory1> GetD2DFactory() { return m_instance.m_d2dFactory; }
    static IDWriteFactory* GetDWriteFactory() { return m_instance.m_dwriteFactory.Get(); }
	static IDWriteFactory3* GetDWriteFactory3() { return m_instance.m_dwriteFactory3.Get(); }
	static Microsoft::WRL::ComPtr<ID2D1DeviceContext> GetD2DContextWrapped() { return m_instance.m_d2dContext; }
	static ID2D1DeviceContext* GetD2DContext() { return m_instance.m_d2dContext.Get(); }
	static IDWriteFontCollection* GetFontCollection() {	return m_instance.m_fontCollection.Get(); }

    static const MFARGB Color(D2D1::ColorF::Enum color, BYTE alpha = (0xff));
    static const D2D1_SIZE_F Snug2Max(const D2D1_SIZE_F availableArea, const D2D1_SIZE_F requiredArea);
    static void ReleaseDeviceDependentResources();

    void Initialize(
        _In_ ID2D1Device*			d2dDevice,
        _In_ ID2D1DeviceContext*	d2dContext,
        _In_ IWICImagingFactory*	wicFactory,
        _In_ IDWriteFactory*		dwriteFactory,
		_In_ IDWriteFactory3*		dwriteFactory3,
		_In_ IDWriteFontCollection*	drwiteFontCollection
	);

	void Release()
	{
		m_d2dFactory = nullptr;
		m_d2dDevice = nullptr;
		m_d2dContext = nullptr;
		m_dwriteFactory = nullptr;
		m_dwriteFactory3 = nullptr;
		m_stateBlock = nullptr;
		m_wicFactory = nullptr;
	}

	void Update(float timeTotal, float timeDelta);
	void Render(D2D1::Matrix3x2F orientation2D);

	void RegisterElement(blooDot::UIElement elementKey, ElementBase* element);
	ElementBase* GetElement(blooDot::UIElement elementKey);
	void UnregisterElement(blooDot::UIElement elementKey);
	void HitTest(D2D1_POINT_2F point);
	blooDot::UIElement GetSelectedButton();
	blooDot::UIElement PopPressed();
	std::unique_ptr<DialogOverlay> ActiveDialog;

private:
	UserInterface();
    ~UserInterface() {}

    static UserInterface m_instance;

    Microsoft::WRL::ComPtr<ID2D1Factory1>           m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device>             m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext>      m_d2dContext;
    Microsoft::WRL::ComPtr<IDWriteFactory>          m_dwriteFactory;
	Microsoft::WRL::ComPtr<IDWriteFactory3>         m_dwriteFactory3;
	Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>  m_stateBlock;
    Microsoft::WRL::ComPtr<IWICImagingFactory>      m_wicFactory;
	Microsoft::WRL::ComPtr<IDWriteFontCollection>   m_fontCollection;

    ElementSet m_elements;
};
