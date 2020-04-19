#pragma once

class DialogDingSheet : public DialogOverlay
{
public:
	DialogDingSheet();
	void SetContent(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> dingSheet);


protected:
	void RenderClientarea(ID2D1DeviceContext* d2dContext) override;

private:
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> m_dingSheet;
};