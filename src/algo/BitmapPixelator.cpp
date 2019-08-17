#include "..\PreCompiledHeaders.h"
#include "BitmapPixelator.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;
using namespace blooDot;

BitmapPixelator::BitmapPixelator()
{
}

BitmapPixelator::~BitmapPixelator()
{
	delete m_Plane;
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
	HRESULT hr = S_OK;

	IWICImagingFactory* pIWICFactory;
	IWICBitmapDecoder *pIDecoder = NULL;
	IWICBitmapFrameDecode *pIDecoderFrame = NULL;
	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pIWICFactory)
	);

	hr = pIWICFactory->CreateDecoderFromFilename(
		L"turtle.jpg",                  // Image to be decoded
		NULL,                           // Do not prefer a particular vendor
		GENERIC_READ,                   // Desired read access to the file
		WICDecodeMetadataCacheOnDemand, // Cache metadata when needed
		&pIDecoder                      // Pointer to the decoder
	);

	// Retrieve the first bitmap frame.
	if (SUCCEEDED(hr))
	{
		hr = pIDecoder->GetFrame(0, &pIDecoderFrame);
	}

	IWICBitmap *pIBitmap = NULL;
	IWICBitmapLock *pILock = NULL;

	UINT uiWidth = 10;
	UINT uiHeight = 10;

	WICRect rcLock = { 0, 0, uiWidth, uiHeight };

	// Create the bitmap from the image frame.
	if (SUCCEEDED(hr))
	{
		hr = pIWICFactory->CreateBitmapFromSource(
			pIDecoderFrame,          // Create a bitmap from the image frame
			WICBitmapCacheOnDemand,  // Cache metadata when needed
			&pIBitmap);              // Pointer to the bitmap
	}

	if (SUCCEEDED(hr))
	{
		// Obtain a bitmap lock for exclusive write.
		// The lock is for a 10x10 rectangle starting at the top left of the
		// bitmap.
		hr = pIBitmap->Lock(&rcLock, WICBitmapLockWrite, &pILock);
		if (SUCCEEDED(hr))
		{
			UINT cbBufferSize = 0;
			BYTE *pv = NULL;

			// Retrieve a pointer to the pixel data.
			if (SUCCEEDED(hr))
			{
				hr = pILock->GetDataPointer(&cbBufferSize, &pv);
			}

			// Pixel manipulation using the image data pointer pv.
			// ...

			// Release the bitmap lock.
			SafeRelease(&pILock);
		}
	}

	SafeRelease(&pIBitmap);
	SafeRelease(&pIDecoder);
	SafeRelease(&pIDecoderFrame);
}
