#pragma once

struct D2D1_COLOR_FCompare
{
	bool operator() (const MFARGB& lhs, const MFARGB& rhs) const
	{
		long lLhs = (lhs.rgbAlpha << 24) + (lhs.rgbRed << 16) + (lhs.rgbGreen << 8) + lhs.rgbBlue;
		long lRhs = (rhs.rgbAlpha << 24) + (rhs.rgbRed << 16) + (rhs.rgbGreen << 8) + rhs.rgbBlue;
		return lLhs < lRhs;
	}
};

class BrushRegistry
{
public:
	BrushRegistry();
	~BrushRegistry();

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> WannaHave(Microsoft::WRL::ComPtr<ID2D1DeviceContext> dxDC, MFARGB color);
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> WannaHave(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> dxTarget, MFARGB color);

	void Reset();

private:
	std::map<MFARGB, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>, D2D1_COLOR_FCompare> m_Registry;
};

