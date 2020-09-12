#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

TextButton::TextButton() :
	m_selected(false),
	m_pressed(false),
	m_padding(D2D1_SIZE_F())
{
}

void TextButton::Initialize()
{
	TextElement::Initialize();
}

void TextButton::Update(float timeTotal, float timeDelta)
{
	TextElement::Update(timeTotal, timeDelta);
}

void TextButton::Render()
{
	ID2D1DeviceContext* d2dContext = UserInterface::GetD2DContext();

	if (this->m_selected)
	{
		D2D1_RECT_F bounds = this->GetBounds();
		d2dContext->FillRectangle(bounds, this->m_selectionBackdropBrush.Get());
		d2dContext->DrawRectangle(bounds, this->m_textColorBrush.Get(), 3.f);
	}

	TextElement::Render();
}	

void TextButton::SetPadding(D2D1_SIZE_F padding)
{
	m_padding = padding;
}

void TextButton::SetSelected(bool selected)
{
	m_selected = selected;
}

void TextButton::SetPressed(bool pressed)
{
	m_pressed = pressed;
}

void TextButton::CalculateSize()
{
	TextElement::CalculateSize();
	m_textExtents.left -= m_padding.width;
	m_textExtents.top -= m_padding.height;
	m_size.width += m_padding.width * 2;
	m_size.height += m_padding.height * 2;
}