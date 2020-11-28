#pragma once

class DialogEmbedded : public ElementBase
{
public:
	DialogEmbedded();

	virtual void Initialize(std::shared_ptr<BrushRegistry> brushRegistry) override;
	virtual void Update(float timeTotal, float timeDelta) override;
	virtual void Render() override;
	void ScheduleDialogCommand(blooDot::DialogCommand dialogCommand);
	virtual void ReleaseDeviceDependentResources() override;

protected:
	blooDot::DialogCommand DequeDialogCommand();

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_shadowColorBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_textColorBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_blackBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_coverBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_chromeBrush;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>       m_textLayout;
	D2D1_RECT_F										m_textExtents;
	TextStyle										m_textStyle;
	D2D1_RECT_F										m_clientArea;

private:
	D2D1_RECT_F										m_outerBounds;
	blooDot::DialogCommand							m_pendingDialogCommand;
};