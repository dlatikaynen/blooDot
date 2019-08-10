#pragma once

#include "..\PreCompiledHeaders.h"
#include <DirectXColors.h> // For named colors
#include "..\dx\DeviceResources.h"
#include "..\dx\DirectXHelper.h" // For ThrowIfFailed
#include "GameOfLifePlane.h"

using namespace Windows::Gaming::Input;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::System::Diagnostics;

class GameOfLifeSprinkler
{
public:
	GameOfLifeSprinkler::GameOfLifeSprinkler();
	GameOfLifeSprinkler::~GameOfLifeSprinkler();

	void Create(int radius);
	void Render(Microsoft::WRL::ComPtr<ID2D1DeviceContext> dxDC, int midPointX, int midPointY, int cellSideLength, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush);

protected:
	bool PointInCircle(D2D1_POINT_2L point);

	int m_Radius;
	int m_sideLength;
	int m_radiusSquared;

private:
	GameOfLifePlane* m_Square;

};
