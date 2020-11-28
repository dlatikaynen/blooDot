#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

ElementBase::ElementBase() :
	m_visible(false),
	m_isFadingOut(false)
{
	this->m_alignment.horizontal = AlignCenter;
	this->m_alignment.vertical = AlignCenter;
	this->m_clientAreaContent = D2D1::SizeF(0.f, 0.f);
	this->m_clientAreaScrollOffset = D2D1::Point2F(0.f, 0.f);

}

void ElementBase::Initialize(std::shared_ptr<BrushRegistry> brushRegistry)
{
	this->m_brushRegistry = brushRegistry;
}

void ElementBase::SetAlignment(AlignType horizontal, AlignType vertical)
{
	m_alignment.horizontal = horizontal;
	m_alignment.vertical = vertical;
}

void ElementBase::SetContainer(const D2D1_RECT_F& container)
{
	m_container = container;
}

void ElementBase::SetVisible(bool visible)
{
	if (!(visible && m_isFadingOut))
	{
		m_visible = visible;
	}
}

D2D1_RECT_F ElementBase::GetBounds()
{
	this->CalculateSize();

	D2D1_RECT_F bounds = D2D1::RectF();

	switch (this->m_alignment.horizontal)
	{
	case AlignNear:
		bounds.left = m_container.left;
		bounds.right = bounds.left + m_size.width;
		break;

	case AlignCenter:
		bounds.left = m_container.left + (m_container.right - m_container.left - m_size.width) / 2.0f;
		bounds.right = bounds.left + m_size.width;
		break;

	case AlignFar:
		bounds.right = m_container.right;
		bounds.left = bounds.right - m_size.width;
		break;
	}

	switch (m_alignment.vertical)
	{
	case AlignNear:
		bounds.top = m_container.top;
		bounds.bottom = bounds.top + m_size.height;
		break;

	case AlignCenter:
		bounds.top = m_container.top + (m_container.bottom - m_container.top - m_size.height) / 2.0f;
		bounds.bottom = bounds.top + m_size.height;
		break;

	case AlignFar:
		bounds.bottom = m_container.bottom;
		bounds.top = bounds.bottom - m_size.height;
		break;
	}

	return bounds;
}