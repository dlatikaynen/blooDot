#pragma once

class DialogOverlay : public ElementBase
{
public:
	DialogOverlay();

	virtual void Initialize(std::shared_ptr<BrushRegistry> brushRegistry) override;
	virtual void SetVisible(bool visible) override;
	virtual void Update(float timeTotal, float timeDelta) override;
	virtual void Render() override;
	virtual void ReleaseDeviceDependentResources() override;

	virtual Platform::String^ GetCaption() { return this->m_Caption; }
	void SetCaption(Platform::String^ value);
	void ScheduleDialogCommand(blooDot::DialogCommand dialogCommand, float commandArgument = (0.f));

protected:
	static constexpr float CHROMEWIDTH = 8.0F;
	static constexpr float CAPTIONHEIGHT = 20.0F;

	void SetClientareaSize(D2D1_SIZE_F clientArea);
	virtual void CalculateSize() override;
	virtual Platform::String^ StaticCaption();
	virtual void RenderClientarea(ID2D1DeviceContext* d2dContext);
	blooDot::DialogCommand DequeDialogCommand();

	Microsoft::WRL::ComPtr<ID2D1Brush>			m_shadowColorBrush;
	Microsoft::WRL::ComPtr<ID2D1Brush>			m_textColorBrush;
	Microsoft::WRL::ComPtr<ID2D1Brush>			m_blackBrush;
	Microsoft::WRL::ComPtr<ID2D1Brush>			m_coverBrush;
	Microsoft::WRL::ComPtr<ID2D1Brush>			m_chromeBrush;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>	m_textLayout;
	D2D1_RECT_F									m_textExtents;
	TextStyle									m_textStyle;

private:
	void CreateTextLayout();

	Platform::String^								m_Caption;
	D2D1_RECT_F										m_outerBounds;
	D2D1_SIZE_F										m_sizeClientarea;
	blooDot::DialogCommand							m_pendingDialogCommand;
};