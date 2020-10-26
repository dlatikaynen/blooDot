#pragma once

class TextButton : public TextElement
{
public:
	TextButton();

	virtual void Initialize(std::shared_ptr<BrushRegistry> brushRegistry);
	virtual void Update(float timeTotal, float timeDelta);
	virtual void Render();

	void SetPadding(D2D1_SIZE_F padding);
	void SetSelected(bool selected);

	bool GetSelected() const { return m_selected; }

	void SetPressed(bool pressed);
	bool IsPressed() const { return m_pressed; }

protected:
	virtual void CalculateSize();

	D2D1_SIZE_F     m_padding;
	bool            m_selected;
	bool            m_pressed;
};