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
        (*iterator).second->ReleaseDeviceDependentResources();
    }
}

UserInterface::UserInterface()
{
	this->ActiveDialog = nullptr;
}

void UserInterface::Initialize(
    _In_ ID2D1Device*			d2dDevice,
    _In_ ID2D1DeviceContext*	d2dContext,
    _In_ IWICImagingFactory*	wicFactory,
    _In_ IDWriteFactory*		dwriteFactory,
	_In_ IDWriteFactory3*		dwriteFactory3,
	_In_ IDWriteFontCollection*	dwriteFontCollection
)
{
	this->m_wicFactory = wicFactory;
	this->m_dwriteFactory = dwriteFactory;
	this->m_d2dDevice = d2dDevice;
	this->m_d2dContext = d2dContext;
	this->m_fontCollection = dwriteFontCollection;

    ComPtr<ID2D1Factory> factory;
    d2dDevice->GetFactory(&factory);
    DX::ThrowIfFailed(factory.As(&this->m_d2dFactory));
    DX::ThrowIfFailed(this->m_d2dFactory->CreateDrawingStateBlock(&this->m_stateBlock));
}

void UserInterface::Update(float timeTotal, float timeDelta)
{
    for (auto iter = m_elements.begin(); iter != m_elements.end(); ++iter)
    {
        (*iter).second->Update(timeTotal, timeDelta);
    }
}

void UserInterface::Render(D2D1::Matrix3x2F orientation2D)
{
	this->m_d2dContext->SaveDrawingState(this->m_stateBlock.Get());
	this->m_d2dContext->BeginDraw();
	this->m_d2dContext->SetTransform(orientation2D);
	this->m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    for (auto iter = this->m_elements.begin(); iter != this->m_elements.end(); ++iter)
    {
		if ((*iter).second->IsVisible())
		{
			(*iter).second->Render();
		}
    }

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = this->m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    this->m_d2dContext->RestoreDrawingState(this->m_stateBlock.Get());
}

void UserInterface::RegisterElement(blooDot::UIElement elementKey, ElementBase* element)
{
	this->m_elements.emplace(elementKey, element);
}

ElementBase* UserInterface::UserInterface::GetElement(blooDot::UIElement elementKey)
{
	return this->m_elements[elementKey];
}

void UserInterface::UnregisterElement(blooDot::UIElement elementKey)
{
    this->m_elements.erase(elementKey);
}

inline bool PointInRect(D2D1_POINT_2F point, D2D1_RECT_F rect)
{
    if ((point.x < rect.left) ||
        (point.x > rect.right) ||
        (point.y < rect.top) ||
        (point.y > rect.bottom)
	)
    {
        return false;
    }

    return true;
}

void UserInterface::HitTest(D2D1_POINT_2F point)
{
    for (auto iter = m_elements.begin(); iter != m_elements.end(); ++iter)
    {
		if (!(*iter).second->IsVisible())
		{
			continue;
		}

        TextButton* textButton = dynamic_cast<TextButton*>((*iter).second);
        if (textButton != nullptr)
        {
            D2D1_RECT_F bounds = (*iter).second->GetBounds();
            textButton->SetPressed(PointInRect(point, bounds));
        }
    }
}

const MFARGB UserInterface::Color(D2D1::ColorF::Enum color, BYTE alpha)
{	
	return MFARGB
	{
		static_cast<BYTE>(color & 0x0000ff), //blue
		static_cast<BYTE>((color & 0x00ff00) >> 8), //green
		static_cast<BYTE>((color & 0xff0000) >> 16), //red
		alpha
	};
}

const D2D1_SIZE_F UserInterface::Snug2Max(D2D1_SIZE_F availableArea, D2D1_SIZE_F requiredArea)
{
	auto availableWidth = availableArea.width - blooDot::Consts::SQUARE_WIDTH;
	auto availableHeight = availableArea.height - blooDot::Consts::SQUARE_HEIGHT;
	auto grantedWidth = requiredArea.width;
	auto grantedHeight = requiredArea.height;
	if (requiredArea.width > availableWidth)
	{
		grantedWidth = availableWidth;
	}

	if (requiredArea.height > availableHeight)
	{
		grantedHeight = availableHeight;
	}

	return D2D1::SizeF(grantedWidth, grantedHeight);
	
}

blooDot::UIElement UserInterface::GetSelectedButton()
{
	for (auto iter = this->m_elements.begin(); iter != this->m_elements.end(); ++iter)
	{
		if ((*iter).second->IsVisible())
		{
			auto textButton = dynamic_cast<TextButton*>((*iter).second);
			if (textButton != nullptr && textButton->GetSelected())
			{
				return (*iter).first;
			}
		}
	}

	return blooDot::UIElement::None;
}

blooDot::UIElement UserInterface::PopPressed()
{
	for (auto iter = this->m_elements.begin(); iter != this->m_elements.end(); ++iter)
	{
		if ((*iter).second->IsVisible())
		{
			auto textButton = dynamic_cast<TextButton*>((*iter).second);
			if (textButton != nullptr && textButton->IsPressed())
			{
				textButton->SetPressed(false);
				return (*iter).first;
			}
		}
	}

	return blooDot::UIElement::None;
}
