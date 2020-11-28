#pragma once

class ControlPrimitive
{
public:
	static void DrawScrollIndicators(
		Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext,
		Microsoft::WRL::ComPtr<ID2D1Factory1> d2dFactory,
		std::shared_ptr<BrushRegistry> brushRegistry,
		D2D1_RECT_F clientArea,
		ElementBase::Directions drawSides
	);


};
