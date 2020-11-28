#pragma once

class ElementBase
{
public:
	virtual void Initialize(std::shared_ptr<BrushRegistry> brushRegistry);
	virtual void Update(float timeTotal, float timeDelta) { }
	virtual void Render() { }
	virtual void ReleaseDeviceDependentResources() { }

	void SetAlignment(AlignType horizontal, AlignType vertical);
	virtual void SetContainer(const D2D1_RECT_F& container);
	void SetVisible(bool visible);

	D2D1_RECT_F GetBounds();

	bool IsVisible() const { return m_visible; }

protected:
	ElementBase();

	virtual void CalculateSize() { }

	std::shared_ptr<BrushRegistry>	m_brushRegistry;
	Alignment						m_alignment;
	D2D1_RECT_F						m_container;
	D2D1_SIZE_F						m_size;
	D2D1_SIZE_F						m_clientAreaContent;
	D2D1_POINT_2F					m_clientAreaScrollOffset;
	bool							m_visible;
	bool							m_isFadingOut;
};

typedef std::map<blooDot::UIElement, ElementBase*> ElementSet;