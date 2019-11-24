#include "..\PreCompiledHeaders.h"
#include <DirectXColors.h> // For named colors
#include <intrin.h>
#include "..\app\blooDot.h"
#include "Level.h"

using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;
using namespace std;
using namespace concurrency;

Level::Level(Platform::String^ levelName, D2D1_SIZE_U sheetSize, unsigned extentWE, unsigned extentNS)
{
	this->m_rectangularBounds = D2D1::SizeU(extentWE, extentNS);
	this->m_sheetSize = sheetSize;
	this->m_Name = levelName;
	this->Clear();
	this->m_isDesignTime = false;
	this->m_DesignTimeDirty = false;
}

Level::~Level()
{
	while (!this->m_Objects.empty()) 
	{
		delete this->m_Objects.back();
		this->m_Objects.pop_back();
	}

	this->m_dingSheet.Reset();
	this->m_dingImage.Reset();
	this->m_floorBackground.Reset();
}

void Level::Clear()
{
	if (!this->m_Objects.empty())
	{
		this->m_Objects.clear();
	}

	/* generates the matrix for this level, lazy-loading */
	for (unsigned y = 0; y < this->m_rectangularBounds.height; ++y)
	{
		for (unsigned x = 0; x < this->m_rectangularBounds.width; ++x)
		{
			this->m_Objects.push_back((Object*)nullptr);
		}
	}

	if (m_isDesignTime)
	{
		this->m_DesignTimeDirty = true;
	}
}

void Level::Initialize(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* brushRegistry)
{	
	auto resources = deviceResources->GetD3DDevice();
	auto device = deviceResources->GetD2DDeviceContext();
	auto loader = ref new BasicLoader(resources);
	loader->LoadPngToBitmap(L"Media\\Bitmaps\\universe_seamless.png", deviceResources, &this->m_floorBackground);

	auto deflt = Dings(0, "BLACK", deviceResources, brushRegistry);
	auto mauer = Mauer(deviceResources, brushRegistry);
	auto wasser = Wasser(deviceResources, brushRegistry);
	auto hgrass = HighGrass(deviceResources, brushRegistry);
	auto snow = Snow(deviceResources, brushRegistry);
	auto dalek = Dalek(deviceResources, brushRegistry);

	DX::ThrowIfFailed(device->CreateCompatibleRenderTarget(D2D1::SizeF(800.0f, 600.0f), &this->m_dingSheet));
	DX::ThrowIfFailed(device->CreateCompatibleRenderTarget(D2D1::SizeF(blooDot::Consts::SQUARE_WIDTH, blooDot::Consts::SQUARE_HEIGHT), &this->m_dingImage));
	m_dingSheet->BeginDraw();

	deflt.Draw(m_dingSheet, 0, 0);
	mauer.Draw(m_dingSheet, 1, 1);
	wasser.Draw(m_dingSheet, 1, 0);
	hgrass.Draw(m_dingSheet, 2, 0);
	snow.Draw(m_dingSheet, 3, 0);
	dalek.Draw(m_dingSheet, 4, 0);
	DX::ThrowIfFailed(m_dingSheet->EndDraw());

	this->m_dingMap.emplace(deflt.ID(), deflt);
	this->m_dingMap.emplace(mauer.ID(), mauer);
	this->m_dingMap.emplace(wasser.ID(), wasser);
	this->m_dingMap.emplace(hgrass.ID(), hgrass);
	this->m_dingMap.emplace(snow.ID(), snow);
	this->m_dingMap.emplace(dalek.ID(), dalek);
}

Dings* Level::GetDing(unsigned dingID)
{
	return &(this->m_dingMap.at(dingID));
}

unsigned Level::GetPreviousDingID(unsigned dingID)
{
	auto dingPointer = this->m_dingMap.find(dingID);
	if (dingPointer == this->m_dingMap.end())
	{
		return 0;
	}
	else
	{
		if ((--dingPointer) == this->m_dingMap.begin())
		{
			return 0;
		}
		else
		{
			return dingPointer->second.ID();
		}
	}
}

unsigned Level::GetNextDingID(unsigned dingID)
{
	auto dingPointer = this->m_dingMap.find(dingID);	
	if (dingPointer == this->m_dingMap.end())
	{
		return 0;
	}
	else
	{
		if ((++dingPointer) == this->m_dingMap.end())
		{
			return 0;
		}
		else
		{
			return dingPointer->second.ID();
		}
	}
}

unsigned Level::ConfirmDingID(unsigned dingID)
{
	return this->m_dingMap.count(dingID) ? dingID : 0;
}

Microsoft::WRL::ComPtr<ID2D1Bitmap> Level::CreateDingImage(unsigned dingID)
{
	auto dingToRender = this->GetDing(dingID);
	auto dingOnSheet = dingToRender->GetSheetPlacement(Facings::Shy);
	ID2D1Bitmap *sheetSurface = NULL;
	ID2D1Bitmap *resultBitmap = NULL;

	m_dingImage->BeginDraw();
	m_dingImage->Clear();
	this->m_dingSheet->GetBitmap(&sheetSurface);
	D2D1_RECT_F dingRect = D2D1::RectF(dingOnSheet.x * 49.0f, dingOnSheet.y * 49.0f, dingOnSheet.x * 49.0f + 49.0f, dingOnSheet.y * 49.0f + 49.0f);
	D2D1_RECT_F placementRect = D2D1::RectF(0, 0, blooDot::Consts::SQUARE_WIDTH, blooDot::Consts::SQUARE_HEIGHT);
	m_dingImage->DrawBitmap(sheetSurface, placementRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, dingRect);
	DX::ThrowIfFailed(m_dingImage->EndDraw());
	m_dingImage->GetBitmap(&resultBitmap);
	Microsoft::WRL::ComPtr<ID2D1Bitmap> resultPointer = resultBitmap;
	return resultPointer;
}

unsigned Level::GetNumOfSheetsWE()
{
	return this->GetNumOfSheetsRequired(this->m_rectangularBounds.width, this->m_sheetSize.width);
}

unsigned Level::GetNumOfSheetsNS()
{
	return this->GetNumOfSheetsRequired(this->m_rectangularBounds.height, this->m_sheetSize.height);
}

D2D1_SIZE_U Level::GetRectBoundsUnits()
{
	return this->m_rectangularBounds;
}

D2D1_SIZE_U Level::GetSheetSizeUnits()
{
	return this->m_sheetSize;
}

Object* Level::GetObjectAt(unsigned levelX, unsigned levelY, bool createIfNull)
{
	auto objectAddress = levelY * this->m_rectangularBounds.width + levelX;
	auto retrievedObject = (Object*)nullptr;

	/* we create lazily, on demand */
	if (objectAddress >= 0 && objectAddress < this->m_Objects.size())
	{
		retrievedObject = this->m_Objects[objectAddress];
		if (retrievedObject == nullptr && createIfNull)
		{
			auto newObject = new Object(levelX, levelY);
			this->m_Objects[objectAddress] = newObject;
			retrievedObject = newObject;
			if (this->m_isDesignTime)
			{
				this->m_DesignTimeDirty = true;
			}
		}
	}

	return retrievedObject;
}

bool Level::WeedObjectAt(unsigned levelX, unsigned levelY)
{
	auto objectAddress = levelY * this->m_rectangularBounds.width + levelX;
	if (objectAddress >= 0 && objectAddress < this->m_Objects.size())
	{
		auto existingObject = this->m_Objects[objectAddress];
		if (existingObject != nullptr)
		{
			existingObject->Weed();
			this->m_Objects[objectAddress] = nullptr;
			if (this->m_isDesignTime)
			{
				this->m_DesignTimeDirty = true;
			}
			return true;
		}
	}

	return false;
}

ClumsyPacking::NeighborConfiguration Level::GetNeighborConfigurationOf(unsigned levelX, unsigned levelY, unsigned dingID, Layers inLayer)
{
	if (dingID > 0)
	{
		return ClumsyPacking::ConfigurationFromNeighbors(
			this->HasCompatibleNeighbor((int)levelX - 1, (int)levelY - 1, dingID, inLayer),
			this->HasCompatibleNeighbor((int)levelX, (int)levelY - 1, dingID, inLayer),
			this->HasCompatibleNeighbor((int)levelX + 1, (int)levelY - 1, dingID, inLayer),
			this->HasCompatibleNeighbor((int)levelX + 1, (int)levelY, dingID, inLayer),
			this->HasCompatibleNeighbor((int)levelX + 1, (int)levelY + 1, dingID, inLayer),
			this->HasCompatibleNeighbor((int)levelX, (int)levelY + 1, dingID, inLayer),
			this->HasCompatibleNeighbor((int)levelX - 1, (int)levelY + 1, dingID, inLayer),
			this->HasCompatibleNeighbor((int)levelX - 1, (int)levelY, dingID, inLayer)
		);
	}

	return Facings::Shy;
}

bool Level::HasCompatibleNeighbor(int x, int y, int dingID, Layers ofLayer)
{
	if (!(x < 0 || y < 0 || x >= this->m_rectangularBounds.width || y >= this->m_rectangularBounds.height))
	{
		auto thereObject = this->GetObjectAt(x, y, false);
		if (thereObject != nullptr && ((thereObject->GetLayers() & ofLayer) == ofLayer))
		{
			auto thereDing = thereObject->GetDing(ofLayer);
			if (thereDing != nullptr && thereDing->ID() == dingID)
			{
				return true;
			}
		}
	}

	return false;
}

Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> Level::GetDingSheet()
{
	return this->m_dingSheet;
}

unsigned Level::GetNumOfSheetsRequired(unsigned extentUnits, unsigned sizePerSheet)
{
	return static_cast<unsigned>(ceilf(static_cast<float>(extentUnits) / static_cast<float>(sizePerSheet)));
}

Microsoft::WRL::ComPtr<ID2D1Bitmap> Level::GetFloorBackground()
{
	return this->m_floorBackground;
}

void Level::SetDesignTime()
{
	this->m_isDesignTime = true;
}

bool Level::HasSaveFileNameBeenSpecifiedBefore()
{
	return !this->m_lastSavedAsFileName->IsEmpty();
}

void Level::DesignSaveToFile()
{
	if (this->HasSaveFileNameBeenSpecifiedBefore())
	{
		this->DesignSaveToFile(this->m_lastSavedAsFileName);
	}
}

void Level::DesignSaveToFile(Platform::String^ fileName)
{
	unsigned blockSaveCount = 0;
	unsigned char whatsthere;

	std::wstring fName = fileName->Data();
	std::ofstream oF;
	oF.open(fName, ios_base::out | ios_base::binary);
	/* header */
	oF.write((char*)&blooDot::blooDotMain::BLOODOTFILE_SIGNATURE, sizeof(blooDot::blooDotMain::BLOODOTFILE_SIGNATURE));
	oF.write((char *)&sigbyte, sizeof(unsigned char));
	oF.write((char*)&blooDot::blooDotMain::BLOODOTFILE_CONTENTTYPE_LEVEL_DESIGN, sizeof(blooDot::blooDotMain::BLOODOTFILE_CONTENTTYPE_LEVEL_DESIGN));
	/* dimensions */
	oF.write((char *)&m_rectangularBounds.height, sizeof(uint32));
	oF.write((char *)&m_rectangularBounds.width, sizeof(uint32));
	/* skip table */
	for (unsigned y = 0; y < this->m_rectangularBounds.height; ++y)
	{
		for (unsigned x = 0; x < this->m_rectangularBounds.width; ++x)
		{
			auto objectAddress = y * this->m_rectangularBounds.width + x;
			if (objectAddress >= 0 && objectAddress < this->m_Objects.size())
			{
				auto allocatedObject = this->m_Objects[objectAddress];
				if (allocatedObject == nullptr)
				{
					oF.write((char *)&emptybit, sizeof(const unsigned char));
				}
				else
				{
					whatsthere = 0x0;
					auto layers = allocatedObject->GetLayers();
					if ((layers & Layers::Floor) == Layers::Floor)
					{
						whatsthere |= floorbit;
					}

					if ((layers & Layers::Walls) == Layers::Walls)
					{
						whatsthere |= wallsbit;
					}

					if ((layers & Layers::Rooof) == Layers::Rooof)
					{
						whatsthere |= Rooof;
					}

					oF.write((char *)&whatsthere, sizeof(const unsigned char));
					if ((layers & Layers::Floor) == Layers::Floor)
					{
						allocatedObject->GetDing(Layers::Floor)->DesignSaveToFile(&oF);
					}

					if ((layers & Layers::Walls) == Layers::Walls)
					{
						allocatedObject->GetDing(Layers::Walls)->DesignSaveToFile(&oF);
					}

					if ((layers & Layers::Rooof) == Layers::Rooof)
					{
						allocatedObject->GetDing(Layers::Rooof)->DesignSaveToFile(&oF);
					}

#ifdef _DEBUG
					if (whatsthere > 0)
					{
						++blockSaveCount;
					}
#endif
				}
			}
		}
	}

	oF.flush();
	oF.close();

#ifdef _DEBUG
	const int bufferLength = 16;
	char16 str[bufferLength];
	int len = swprintf_s(str, bufferLength, L"%d", blockSaveCount);
	Platform::String^ string = ref new Platform::String(str, len);
	OutputDebugStringW(Platform::String::Concat(Platform::String::Concat(L"Number of non-empty squares saved: ", string), L"\r\n")->Data());
#endif

	this->m_DesignTimeDirty = false;
	this->m_lastSavedAsFileName = fileName;
}

bool Level::DesignLoadFromFile(Platform::String^ fileName)
{
	BasicReaderWriter^ basicReaderWriter;
	unsigned long long offset = 0L;
#ifdef _DEBUG
	unsigned blockLoadCount = 0;
#endif	

	std::ifstream iF;
	iF.open(fileName->Data(), ios_base::in | ios_base::binary);
	if (!iF.fail() && iF.is_open())
	{
		iF.seekg(0, std::ios::end);
		size_t length = iF.tellg();
		iF.seekg(0, std::ios::beg);
		char* srcData = new char[length];
		iF.read(srcData, length);

		//Platform::Array<byte>^ rawData = basicReaderWriter->ReadData(fileName);
		//byte* srcData = rawData->Data;
		/* parse the signature */
		uint64 fileSignature = *reinterpret_cast<uint64*>(srcData + offset); offset += sizeof(uint64);
		uint64 signatToMatch = (uint64_t)(*(uint64_t*)&blooDot::blooDotMain::BLOODOTFILE_SIGNATURE[0]);
		if (fileSignature != signatToMatch)
		{
			return false;
		}

		unsigned char sigByte = *reinterpret_cast<unsigned char*>(srcData + offset); offset += sizeof(unsigned char);
		if (sigByte != sigbyte)
		{
			return false;
		}

		uint16 fileType = *reinterpret_cast<uint16*>(srcData + offset); offset += sizeof(uint16);
		uint64 tToMatch = (uint16_t)(*(uint16_t*)&blooDot::blooDotMain::BLOODOTFILE_CONTENTTYPE_LEVEL_DESIGN[0]);
		if (fileType != tToMatch)
		{
			return false;
		}

		/* read and apply the size */
		unsigned short extentY = *reinterpret_cast<uint32*>(srcData + offset); offset += sizeof(uint32);
		unsigned short extentX = *reinterpret_cast<uint32*>(srcData + offset); offset += sizeof(uint32);
		this->m_rectangularBounds.width = extentX;
		this->m_rectangularBounds.height = extentY;
		this->Clear();
		/* read the matrix */
		for (unsigned y = 0; y < this->m_rectangularBounds.height; ++y)
		{
			for (unsigned x = 0; x < this->m_rectangularBounds.width; ++x)
			{
				auto objectAddress = y * this->m_rectangularBounds.width + x;
				if (objectAddress >= 0 && objectAddress < this->m_Objects.size())
				{
					unsigned char whatsthere = *reinterpret_cast<unsigned char*>(srcData + offset); offset += sizeof(const unsigned char);
					if ((whatsthere & this->floorbit) == this->floorbit)
					{
						unsigned dingIdFloor = *reinterpret_cast<uint32*>(srcData + offset); offset += sizeof(uint32);
						this->GetObjectAt(x, y, true)->InstantiateInLayer(Layers::Floor, &this->m_dingMap.at(dingIdFloor), 0);
					}

					if ((whatsthere & this->wallsbit) == this->wallsbit)
					{
						uint32 dingIdWalls = *reinterpret_cast<uint32*>(srcData + offset); offset += sizeof(uint32);
						this->GetObjectAt(x, y, true)->InstantiateInLayer(Layers::Walls, &this->m_dingMap.at(dingIdWalls), 0);
					}

					if ((whatsthere & this->rooofbit) == this->rooofbit)
					{
						uint32 dingIdRooof = *reinterpret_cast<uint32*>(srcData + offset); offset += sizeof(uint32);
						this->GetObjectAt(x, y, true)->InstantiateInLayer(Layers::Rooof, &this->m_dingMap.at(dingIdRooof), 0);
					}

#ifdef _DEBUG
					blockLoadCount += (whatsthere & this->emptybit) == this->emptybit ? 0 : 1;
#endif
				}
			}
		}

		delete[] srcData;
		/* so next time hitting "Save" will not prompt for file name */
		this->m_lastSavedAsFileName = fileName;
		this->m_DesignTimeDirty = false;
	}

#ifdef _DEBUG
	const int bufferLength = 16;
	char16 str[bufferLength];
	int len = swprintf_s(str, bufferLength, L"%d", blockLoadCount);
	Platform::String^ string = ref new Platform::String(str, len);
	OutputDebugStringW(Platform::String::Concat(Platform::String::Concat(L"Number of non-empty squares loaded: ", string), L"\r\n")->Data());
#endif

	return true;
}
