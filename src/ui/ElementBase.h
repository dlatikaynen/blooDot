#pragma once

class ElementBase
{
public:
	enum Directions : unsigned int
	{
		NONE = 0,
		LeftWest = 1 << 0,
		RightEast = 1 << 1,
		DownSouth = 1 << 2,
		UpNorth = 1 << 3
	};

	// another kind of finite simple group,
	// obviously a subset of the clumsy pack
	enum EdgeCoalescingCases : unsigned int
	{
		NoEdge = 0,
		SingleWest = Directions::LeftWest,
		SingleEast = Directions::RightEast,
		SingleSouth = Directions::DownSouth,
		SingleNorth = Directions::UpNorth,
		TwinNW = 16,
		TwinNE = 17,
		TwinSE = 18,
		TwinSW = 19,
		TripletWNE = 31,
		TripletNWS = 32,
		TripletNES = 33,
		TripletWSE = 34,
		Plenty = 40
	};

	virtual void Initialize(std::shared_ptr<BrushRegistry> brushRegistry);
	virtual void Update(float timeTotal, float timeDelta) { }
	virtual void Render() { }
	virtual void ReleaseDeviceDependentResources() { }

	void SetAlignment(AlignType horizontal, AlignType vertical);
	virtual void SetContainer(const D2D1_RECT_F& container);
	virtual void SetVisible(bool visible);
	bool IsVisible() const;
	D2D1_RECT_F GetBounds();
	void SetControl(DirectX::XMFLOAT2 pointerPosition, TouchMap* touchMap, bool shiftKeyActive, bool left, bool right, bool up, bool down, float scrollDeltaX, float scrollDeltaY);
	Directions HasScrollOverflow();
	static EdgeCoalescingCases EdgeCoalescingCaseFrom(const Directions edges);
	void Scroll(const float deltaX, const float deltaY);

protected:
	ElementBase();

	virtual void CalculateSize() { }	

	std::shared_ptr<BrushRegistry>	m_brushRegistry;
	Alignment						m_alignment;
	D2D1_RECT_F						m_container;
	D2D1_SIZE_F						m_size;
	D2D1_RECT_F						m_clientArea;
	D2D1_SIZE_F						m_clientAreaContent;
	D2D1_POINT_2F					m_clientAreaScrollOffset;
	bool							m_visible;
	bool							m_isFadingOut;
};

constexpr inline ElementBase::Directions operator |(const ElementBase::Directions left, const ElementBase::Directions& right)
{
	return static_cast<ElementBase::Directions>(static_cast<int>(left) | static_cast<int>(right));
}

ElementBase::Directions& operator |=(ElementBase::Directions& a, ElementBase::Directions b);

constexpr inline ElementBase::EdgeCoalescingCases operator |(const ElementBase::EdgeCoalescingCases left, const ElementBase::EdgeCoalescingCases& right)
{
	return static_cast<ElementBase::EdgeCoalescingCases>(static_cast<int>(left) | static_cast<int>(right));
}

ElementBase::EdgeCoalescingCases& operator |=(ElementBase::EdgeCoalescingCases& a, ElementBase::EdgeCoalescingCases b);

typedef std::map<blooDot::UIElement, ElementBase*> ElementSet;
