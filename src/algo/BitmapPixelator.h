#pragma once

#include "..\PreCompiledHeaders.h"
#include <DirectXColors.h> // For named colors
#include "..\dx\DeviceResources.h"
#include "..\dx\DirectXHelper.h" // For ThrowIfFailed
#include "..\dx\BrushRegistry.h"
#include "GameOfLife.h"

using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::System::Diagnostics;

class BitmapPixelator
{
public:
	BitmapPixelator();
	~BitmapPixelator();

	void Load(std::string fileName);
	void GenerateFromFont(std::string text);
	void PlaceAt(GameOfLifePlane* onPlane, int atX, int atY);

private:
	Microsoft::WRL::ComPtr<ID2D1Bitmap> m_Bitmap;
	GameOfLifePlane* m_Plane;

};

