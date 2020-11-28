#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

ElementBase::Directions& operator |=(ElementBase::Directions& a, ElementBase::Directions b)
{
	return a = a | b;
}

ElementBase::EdgeCoalescingCases& operator |=(ElementBase::EdgeCoalescingCases& a, ElementBase::EdgeCoalescingCases b)
{
	return a = a | b;
}

ElementBase::ElementBase() :
	m_visible(false),
	m_isFadingOut(false)
{
	this->m_alignment.horizontal = AlignCenter;
	this->m_alignment.vertical = AlignCenter;
	this->m_clientArea = D2D1::RectF(0.f, 0.f, 0.f, 0.f);
	this->m_clientAreaContent = D2D1::SizeF(0.f, 0.f);
	this->m_clientAreaScrollOffset = D2D1::Point2F(0.f, 0.f);

}

void ElementBase::Initialize(std::shared_ptr<BrushRegistry> brushRegistry)
{
	this->m_brushRegistry = brushRegistry;
}

bool ElementBase::IsVisible() const
{ 
	return this->m_visible;
}

void ElementBase::SetAlignment(AlignType horizontal, AlignType vertical)
{
	this->m_alignment.horizontal = horizontal;
	this->m_alignment.vertical = vertical;
}

void ElementBase::SetContainer(const D2D1_RECT_F& container)
{
	this->m_container = container;
}

void ElementBase::SetVisible(bool visible)
{
	if (!(visible && this->m_isFadingOut))
	{
		this->m_visible = visible;
	}
}

D2D1_RECT_F ElementBase::GetBounds()
{
	D2D1_RECT_F bounds = D2D1::RectF();
	this->CalculateSize();
	switch (this->m_alignment.horizontal)
	{
	case AlignNear:
		bounds.left = this->m_container.left;
		bounds.right = bounds.left + this->m_size.width;
		break;

	case AlignCenter:
		bounds.left = this->m_container.left + (this->m_container.right - this->m_container.left - this->m_size.width) / 2.f;
		bounds.right = bounds.left + this->m_size.width;
		break;

	case AlignFar:
		bounds.right = this->m_container.right;
		bounds.left = bounds.right - this->m_size.width;
		break;
	}

	switch (this->m_alignment.vertical)
	{
	case AlignNear:
		bounds.top = this->m_container.top;
		bounds.bottom = bounds.top + this->m_size.height;
		break;

	case AlignCenter:
		bounds.top = this->m_container.top + (this->m_container.bottom - this->m_container.top - this->m_size.height) / 2.0f;
		bounds.bottom = bounds.top + this->m_size.height;
		break;

	case AlignFar:
		bounds.bottom = this->m_container.bottom;
		bounds.top = bounds.bottom - this->m_size.height;
		break;
	}

	return bounds;
}

ElementBase::Directions ElementBase::HasScrollOverflow()
{
	ElementBase::Directions result = ElementBase::Directions::NONE;
	if (this->m_clientArea.right > 0.f && this->m_clientAreaContent.width > 0)
	{
		if (this->m_clientAreaScrollOffset.x > 0.f)
		{
			result |= ElementBase::Directions::LeftWest;
		}

		if ((this->m_clientArea.right - this->m_clientArea.left) + this->m_clientAreaScrollOffset.x < this->m_clientAreaContent.width)
		{
			result |= ElementBase::Directions::RightEast;
		}
	}

	if (this->m_clientArea.bottom > 0.f && this->m_clientAreaContent.height > 0)
	{
		if (this->m_clientAreaScrollOffset.y > 0.f)
		{
			result |= ElementBase::Directions::UpNorth;
		}

		if ((this->m_clientArea.bottom - this->m_clientArea.top) + this->m_clientAreaScrollOffset.y < this->m_clientAreaContent.height)
		{
			result |= ElementBase::Directions::DownSouth;
		}
	}

	return result;
}

ElementBase::EdgeCoalescingCases ElementBase::EdgeCoalescingCaseFrom(ElementBase::Directions edges)
{
	auto
		left = edges & ElementBase::Directions::LeftWest,
		rite = edges & ElementBase::Directions::RightEast,
		down = edges & ElementBase::Directions::DownSouth,
		upes = edges & ElementBase::Directions::UpNorth;
	
	if (left && rite && down && upes)
	{
		return EdgeCoalescingCases::Plenty;
	}
	else if (left && upes && rite)
	{
		return EdgeCoalescingCases::TripletWNE;
	}
	else if (left && upes && down)
	{
		return EdgeCoalescingCases::TripletNWS;
	}
	else if (rite && upes && down)
	{
		return EdgeCoalescingCases::TripletNES;
	}
	else if (left && rite && down)
	{
		return EdgeCoalescingCases::TripletWSE;
	}
	else if (left && rite)
	{
		return EdgeCoalescingCases::SingleWest | EdgeCoalescingCases::SingleEast;
	}
	else if (upes && down)
	{
		return EdgeCoalescingCases::SingleNorth | EdgeCoalescingCases::SingleSouth;
	}
	
	return static_cast<EdgeCoalescingCases>(edges);
}