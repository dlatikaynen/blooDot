#pragma once

#include "TextStyle.h"
#include "ElementBase.h"
#include "TextElement.h"
#include "CountdownTimer.h"
#include "StopwatchTimer.h"
#include "NerdStatsDisplay.h"
#include "TextButton.h"
#include "HighScoreTable.h"

class UserInterface
{
public:
    static UserInterface& GetInstance() { return m_instance; }

    static IDWriteFactory* GetDWriteFactory() { return m_instance.m_dwriteFactory.Get(); }
    static ID2D1DeviceContext* GetD2DContext() { return m_instance.m_d2dContext.Get(); }
    static void ReleaseDeviceDependentResources();

    void Initialize(
        _In_ ID2D1Device*         d2dDevice,
        _In_ ID2D1DeviceContext*  d2dContext,
        _In_ IWICImagingFactory*  wicFactory,
        _In_ IDWriteFactory*      dwriteFactory
        );

    void Release()
    {
        m_d2dFactory = nullptr;
        m_d2dDevice = nullptr;
        m_d2dContext = nullptr;
        m_dwriteFactory = nullptr;
        m_stateBlock = nullptr;
        m_wicFactory = nullptr;
    }

    void Update(float timeTotal, float timeDelta);
    void Render(D2D1::Matrix3x2F orientation2D);

    void RegisterElement(ElementBase* element);
    void UnregisterElement(ElementBase* element);

    void HitTest(D2D1_POINT_2F point);

private:
    UserInterface() {}
    ~UserInterface() {}

    static UserInterface m_instance;

    Microsoft::WRL::ComPtr<ID2D1Factory1>           m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device>             m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext>      m_d2dContext;
    Microsoft::WRL::ComPtr<IDWriteFactory>          m_dwriteFactory;
    Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>  m_stateBlock;
    Microsoft::WRL::ComPtr<IWICImagingFactory>      m_wicFactory;

    ElementSet m_elements;
};
