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

void ElementBase::Scroll(const float deltaX, const float deltaY)
{
	if (this->m_clientArea.right > 0.f && this->m_clientAreaContent.width > 0)
	{
		if (deltaX > 0.f)
		{
			if (((this->m_clientArea.right - this->m_clientArea.left) + this->m_clientAreaScrollOffset.x + deltaX) > this->m_clientAreaContent.width)
			{
				this->m_clientAreaScrollOffset.x = this->m_clientAreaContent.width - (this->m_clientArea.right - this->m_clientArea.left);
			}
			else
			{
				this->m_clientAreaScrollOffset.x += deltaX;
			}
		}
		else if (deltaX < 0.f)
		{
			if ((this->m_clientAreaScrollOffset.x + deltaX) < 0.f)
			{
				this->m_clientAreaScrollOffset.x = 0.f;
			}
			else
			{
				this->m_clientAreaScrollOffset.x += deltaX;
			}
		}

		if (deltaY > 0.f)
		{
			if (((this->m_clientArea.bottom - this->m_clientArea.top) + this->m_clientAreaScrollOffset.y + deltaY) > this->m_clientAreaContent.height)
			{
				this->m_clientAreaScrollOffset.y = this->m_clientAreaContent.height - (this->m_clientArea.bottom - this->m_clientArea.top);
			}
			else
			{
				this->m_clientAreaScrollOffset.y += deltaY;
			}
		}
		else if (deltaY < 0.f)
		{
			if ((this->m_clientAreaScrollOffset.y + deltaY) < 0.f)
			{
				this->m_clientAreaScrollOffset.y = 0;
			}
			else
			{
				this->m_clientAreaScrollOffset.y += deltaY;
			}
		}
	}
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
	constexpr static EdgeCoalescingCases lookup[] = {
		EdgeCoalescingCases::NoEdge,                                         // 0
		EdgeCoalescingCases::SingleWest,                                     // 1 ~ ElementBase::Directions::LeftWest
		EdgeCoalescingCases::SingleEast,                                     // 2 ~ ElementBase::Directions::RightEast
		EdgeCoalescingCases::SingleWest | EdgeCoalescingCases::SingleEast,   // 3
		EdgeCoalescingCases::SingleSouth,                                    // 4 ~ ElementBase::Directions::DownSouth
		EdgeCoalescingCases::TwinSW,                                         // 5
		EdgeCoalescingCases::TwinSE,                                         // 6
		EdgeCoalescingCases::TripletWSE,                                     // 7
		EdgeCoalescingCases::SingleNorth,                                    // 8 ~ ElementBase::Directions::UpNorth
		EdgeCoalescingCases::TwinNW,                                         // 9
		EdgeCoalescingCases::TwinNE,                                         // 10
		EdgeCoalescingCases::TripletWNE,                                     // 11
		EdgeCoalescingCases::SingleNorth | EdgeCoalescingCases::SingleSouth, // 12
		EdgeCoalescingCases::TripletNWS,                                     // 13
		EdgeCoalescingCases::TripletNES,                                     // 14
		EdgeCoalescingCases::Plenty                                          // 15
	};

	return lookup[edges];
}
