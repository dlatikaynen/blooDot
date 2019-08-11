#include "..\PreCompiledHeaders.h"
#include <DirectXColors.h> // For named colors
#include "BitmapPixelator.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;

BitmapPixelator::BitmapPixelator()
{
}

BitmapPixelator::~BitmapPixelator()
{
}

void BitmapPixelator::Load(std::string fileName)
{

}

void BitmapPixelator::GenerateFromFont(std::string text)
{

}

void BitmapPixelator::PlaceAt(GameOfLifePlane* onPlane, int atX, int atY)
{
	/* effectively scans the bitmap, placing cells where pixels are, in the same color */

}
