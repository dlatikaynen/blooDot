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
	m_Plane = NULL;
}

BitmapPixelator::~BitmapPixelator()
{
	delete m_Plane;
}

void BitmapPixelator::Load(std::wstring fileName)
{
	HRESULT hr = S_OK;
	IWICImagingFactory* pIWICFactory;
	IWICBitmapDecoder *pIDecoder = NULL;
	IWICBitmapFrameDecode *pIDecoderFrame = NULL;
	IWICBitmap *pIBitmap = NULL;
	IWICBitmapLock *pILock = NULL;
	UINT uiWidth;
	UINT uiHeight;

	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pIWICFactory)
	);

	if (SUCCEEDED(hr))
	{
		LPCWSTR fName = fileName.data();
		hr = pIWICFactory->CreateDecoderFromFilename(
			fName,							// Image to be decoded
			NULL,                           // Do not prefer a particular vendor
			GENERIC_READ,                   // Desired read access to the file
			WICDecodeMetadataCacheOnDemand, // Cache metadata when needed
			&pIDecoder                      // Pointer to the decoder
		);
	}

	// Retrieve the first bitmap frame.
	if (SUCCEEDED(hr))
	{
		hr = pIDecoder->GetFrame(0, &pIDecoderFrame);
	}

	if (SUCCEEDED(hr))
	{
		hr = pIDecoderFrame->GetSize(&uiWidth, &uiHeight);
	}

	if (SUCCEEDED(hr))
	{
		WICRect rcLock = { 0, 0, (int)uiWidth, (int)uiHeight };

		// Create the bitmap from the image frame.
		if (SUCCEEDED(hr))
		{
			hr = pIWICFactory->CreateBitmapFromSource(
				pIDecoderFrame,         // Create a bitmap from the image frame
				WICBitmapCacheOnDemand, // Cache metadata when needed
				&pIBitmap				// Pointer to the bitmap
			);
		}		

		if (SUCCEEDED(hr))
		{
			// Obtain a bitmap lock for reading
			hr = pIBitmap->Lock(&rcLock, WICBitmapLockRead, &pILock);
			if (SUCCEEDED(hr))
			{
				UINT cbBufferSize = 0;
				BYTE *pv = NULL;
				
				/* todo: recognize stride and pixelformat,
				 * currently expecting 24bpp rgb invariantly */

				// Retrieve a pointer to the pixel data
				hr = pILock->GetDataPointer(&cbBufferSize, &pv);
				if (SUCCEEDED(hr))
				{
					// Pixel querying using the image data pointer pv.
					// format is 24bpp RGB
					m_Plane = new GameOfLifePlane(uiWidth, uiHeight);
					m_Plane->Wipe();
					MFARGB color;
					unsigned int x = 0, y = 0, pix = 0;
					for (auto i = 0U; i < cbBufferSize; i += 3U)
					{
						/* for the comical ordering of colors, see
						 * https://en.wikipedia.org/wiki/BMP_file_format#Pixel_format 
						 * this is also interesting in-universe, as there are notions
						 * of colors not unlike our own, that feature prominently in the back story:
						 * sibas... (the color) blue;  just as in "si pia SIBAS ian", he-(what)looks_like-blue-coming_from
						 * alas.... (the color) green; just as in "alasta seakyo", green_ish-orb */
						color.rgbAlpha = 0xff;
						color.rgbGreen = pv[i + 2];
						color.rgbRed = pv[i + 1];
						color.rgbBlue = pv[i];
						
						if (color.rgbRed > 0 || color.rgbGreen > 0 || color.rgbBlue > 0)
						{
							m_Plane->SetAlive(x, y);
							m_Plane->CellAt(x, y)->SetColor(color);
						}

						/* next pixel --> next coordinate */
						if (pix > 0 && (pix % uiWidth) == 0)
						{
							++y;
							x = 0;
						}
						else
						{
							++x;
						}

						++pix;
					}
				}

				blooDot::Consts::SafeRelease(&pILock);
			}
		}
	}

	this->SafeRelease(&pIBitmap);
	this->SafeRelease(&pIDecoder);
	this->SafeRelease(&pIDecoderFrame);
}

void BitmapPixelator::GenerateFromFont(std::wstring text)
{

}

void BitmapPixelator::PlaceAt(GameOfLifePlane* onPlane, int atX, int atY)
{
	int destX;
	int destY;

	if (m_Plane != NULL) 
	{
		for (int x = 0; x < m_Plane->GetWidth(); ++x)
		{
			destX = x + atX;
			for (int y = 0; y < m_Plane->GetHeight(); ++y)
			{
				destY = y + atY;
				if (destX < onPlane->GetWidth() && destY < onPlane->GetHeight())
				{
					if (m_Plane->CellAt(x, y)->IsAlive())
					{
						onPlane->SetAlive(destX, destY);
						onPlane->CellAt(destX, destY)->SetColor(m_Plane->CellAt(x, y)->GetCurrentColor());
					}
				}
			}
		}
	}
}
