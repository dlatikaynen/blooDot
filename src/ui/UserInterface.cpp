#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

UserInterface UserInterface::m_instance;

void UserInterface::ReleaseDeviceDependentResources()
{
    m_instance.Release();

    auto elements = m_instance.m_elements;
    for (auto iterator = elements.begin(); iterator != elements.end(); iterator++)
    {
        (*iterator)->ReleaseDeviceDependentResources();
    }
}

void UserInterface::Initialize(
    _In_ ID2D1Device*         d2dDevice,
    _In_ ID2D1DeviceContext*  d2dContext,
    _In_ IWICImagingFactory*  wicFactory,
    _In_ IDWriteFactory*      dwriteFactory
    )
{
    m_wicFactory = wicFactory;
    m_dwriteFactory = dwriteFactory;
    m_d2dDevice = d2dDevice;
    m_d2dContext = d2dContext;

    ComPtr<ID2D1Factory> factory;
    d2dDevice->GetFactory(&factory);

    DX::ThrowIfFailed(
        factory.As(&m_d2dFactory)
        );

    DX::ThrowIfFailed(
        m_d2dFactory->CreateDrawingStateBlock(&m_stateBlock)
        );
}

void UserInterface::Update(float timeTotal, float timeDelta)
{
    for (auto iter = m_elements.begin(); iter != m_elements.end(); ++iter)
    {
        (*iter)->Update(timeTotal, timeDelta);
    }
}

void UserInterface::Render(D2D1::Matrix3x2F orientation2D)
{
    m_d2dContext->SaveDrawingState(m_stateBlock.Get());
    m_d2dContext->BeginDraw();
    m_d2dContext->SetTransform(orientation2D);

    m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    for (auto iter = m_elements.begin(); iter != m_elements.end(); ++iter)
    {
        if ((*iter)->IsVisible())
            (*iter)->Render();
    }

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    m_d2dContext->RestoreDrawingState(m_stateBlock.Get());
}

void UserInterface::RegisterElement(ElementBase* element)
{
    m_elements.insert(element);
}

void UserInterface::UnregisterElement(ElementBase* element)
{
    auto iter = m_elements.find(element);
    if (iter != m_elements.end())
    {
        m_elements.erase(iter);
    }
}

inline bool PointInRect(D2D1_POINT_2F point, D2D1_RECT_F rect)
{
    if ((point.x < rect.left) ||
        (point.x > rect.right) ||
        (point.y < rect.top) ||
        (point.y > rect.bottom))
    {
        return false;
    }

    return true;
}

void UserInterface::HitTest(D2D1_POINT_2F point)
{
    for (auto iter = m_elements.begin(); iter != m_elements.end(); ++iter)
    {
        if (!(*iter)->IsVisible())
            continue;

        TextButton* textButton = dynamic_cast<TextButton*>(*iter);
        if (textButton != nullptr)
        {
            D2D1_RECT_F bounds = (*iter)->GetBounds();
            textButton->SetPressed(PointInRect(point, bounds));
        }
    }
}