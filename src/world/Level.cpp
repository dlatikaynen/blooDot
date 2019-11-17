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
}

Level::~Level()
{
	while (!this->m_Objects.empty()) 
	{
		delete this->m_Objects.back();
		this->m_Objects.pop_back();
	}

	this->m_dingSheet.Reset();
	this->m_floorBackground.Reset();
}

void Level::Clear()
{
	if (!this->m_Objects.empty())
	{
		this->m_Objects.clear();
	}

	/* generates the matrix for this level, lazy-loading */
	for (auto y = 0; y < this->m_rectangularBounds.height; ++y)
	{
		for (auto x = 0; x < this->m_rectangularBounds.width; ++x)
		{
			this->m_Objects.push_back((Object*)nullptr);
		}
	}
}

void Level::Initialize(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* brushRegistry)
{	
	auto resources = deviceResources->GetD3DDevice();
	auto device = deviceResources->GetD2DDeviceContext();
	auto loader = ref new BasicLoader(resources);
	loader->LoadPngToBitmap(L"Media\\Bitmaps\\universe_seamless.png", deviceResources, &this->m_floorBackground);

	auto deflt = Dings(0, "BLACK", brushRegistry);
	auto mauer = Mauer(brushRegistry);
	auto dalek = Dalek(brushRegistry);

	DX::ThrowIfFailed(device->CreateCompatibleRenderTarget(D2D1::SizeF(800.0f, 600.0f), &this->m_dingSheet));
	m_dingSheet->BeginDraw();

	deflt.Draw(m_dingSheet, 0, 0);
	mauer.Draw(m_dingSheet, 1, 1);
	dalek.Draw(m_dingSheet, 1, 0);
	DX::ThrowIfFailed(m_dingSheet->EndDraw());

	this->m_dingMap.emplace(deflt.ID(), deflt);
	this->m_dingMap.emplace(mauer.ID(), mauer);
	this->m_dingMap.emplace(dalek.ID(), dalek);
}

Dings* Level::GetDing(unsigned dingID)
{
	return &(this->m_dingMap.at(dingID));
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
			return true;
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

void Level::DesignSaveToFile(Platform::String^ fileName)
{
	std::wstring fPath = ApplicationData::Current->RoamingFolder->Path->Data();
	std::wstring sPath = fPath.append(L"\\");
	std::wstring fName = sPath.append(fileName->Data());
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
	for (auto y = 0; y < this->m_rectangularBounds.height; ++y)
	{
		for (auto x = 0; x < this->m_rectangularBounds.width; ++x)
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
					unsigned char whatsthere = 0x0;
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
				}
			}
		}
	}

	oF.flush();
	oF.close();
}


bool Level::DesignLoadFromFile(Platform::String^ fileName)
{
	BasicReaderWriter^ basicReaderWriter;
	unsigned long long offset = 0L;
	
	Platform::Array<byte>^ rawData = basicReaderWriter->ReadData(fileName);
	byte* srcData = rawData->Data;
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
	for (auto y = 0; y < this->m_rectangularBounds.height; ++y)
	{
		for (auto x = 0; x < this->m_rectangularBounds.width; ++x)
		{
			auto objectAddress = y * this->m_rectangularBounds.width + x;
			if (objectAddress >= 0 && objectAddress < this->m_Objects.size())
			{
				unsigned char whatsthere = *reinterpret_cast<unsigned char*>(srcData + offset); offset += sizeof(const unsigned char);


			}
		}
	}
}
