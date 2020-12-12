#pragma once

class ControlPrimitive
{
public:
	static void DrawScrollIndicators(
		Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext,
		Microsoft::WRL::ComPtr<ID2D1Factory1> d2dFactory,
		std::shared_ptr<BrushRegistry> brushRegistry,
		D2D1_RECT_F clientArea,
		ElementBase::Directions drawSides,
		D2D1_POINT_2F scrollDirectionVector
	);

private:
	static void ConstructWedge(
		Microsoft::WRL::ComPtr<ID2D1GeometrySink> borderSink,
		const D2D1_POINT_2F* points,
		bool* openFigure,
		const size_t numPoints,
		const int* pointIndices
	);

	static void ProjectLineOnFrame(D2D1_RECT_F frame, D2D1_POINT_2F linedirection, D2D1_POINT_2F* intersection1, D2D1_POINT_2F* intersection2);
};
