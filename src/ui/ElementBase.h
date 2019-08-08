#pragma once

class ElementBase
{
public:
	virtual void Initialize() { }
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

	Alignment       m_alignment;
	D2D1_RECT_F     m_container;
	D2D1_SIZE_F     m_size;
	bool            m_visible;
};

typedef std::set<ElementBase*> ElementSet;