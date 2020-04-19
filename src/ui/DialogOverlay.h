#pragma once

class DialogOverlay : public ElementBase
{
public:
	DialogOverlay();

	virtual void Initialize() override;
	virtual void Update(float timeTotal, float timeDelta) override;
	virtual void Render() override;
	virtual void ReleaseDeviceDependentResources() override;

	virtual Platform::String^ GetCaption() { return this->m_Caption; }
	void SetCaption(Platform::String^ value) { this->m_Caption = value; }

protected:
	void SetClientareaSize(D2D1_RECT_F clientArea);
	virtual void CalculateSize() override;
	virtual Platform::String^ StaticCaption();
	virtual void RenderClientarea(ID2D1DeviceContext* d2dContext);

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_shadowColorBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_textColorBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_blackBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_coverBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_chromeBrush;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>       m_textLayout;
	D2D1_RECT_F										m_textExtents;
	TextStyle										m_textStyle;

private:
	void CreateTextLayout();

	Platform::String^								m_Caption;
	D2D1_RECT_F										m_sizeClientarea;
};