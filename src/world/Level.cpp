#include <DirectXColors.h>
#include <intrin.h>

#include "..\PreCompiledHeaders.h"
#include "..\app\blooDot.h"
#include "Level.h"
#include "..\app\Utility.h"

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
	this->m_designTimeDirty = false;
}

Level::~Level()
{
	while (!this->m_Blocks.empty()) 
	{
		delete this->m_Blocks.back();
		this->m_Blocks.pop_back();
	}

	if (this->m_dingSheet != nullptr)
	{
		this->m_dingSheet.Reset();
		this->m_dingSheet = nullptr;
	}

	if (this->m_mobsSheet != nullptr)
	{
		this->m_mobsSheet.Reset();
		this->m_mobsSheet = nullptr;
	}

	if (this->m_dingSheetBmp != nullptr)
	{
		this->m_dingSheetBmp.Reset();
		this->m_dingSheetBmp = nullptr;
	}

	if (this->m_mobsSheetBmp != nullptr)
	{
		this->m_mobsSheetBmp.Reset();
		this->m_mobsSheetBmp = nullptr;
	}

	if (this->m_dingImage != nullptr)
	{
		this->m_dingImage.Reset();
		this->m_dingImage = nullptr;
	}

	if (this->m_mobsImage != nullptr)
	{
		this->m_mobsImage.Reset();
		this->m_mobsImage = nullptr;
	}

	if (this->m_floorBackground != nullptr)
	{
		this->m_floorBackground.Reset();
		this->m_floorBackground = nullptr;
	}
}

void Level::Clear()
{
	if (!this->m_Blocks.empty())
	{
		this->m_Blocks.clear();
	}

	/* generates the matrix for this level, lazy-loading */
	for (unsigned y = 0; y < this->m_rectangularBounds.height; ++y)
	{
		for (unsigned x = 0; x < this->m_rectangularBounds.width; ++x)
		{
			this->m_Blocks.push_back((Blocks*)nullptr);
		}
	}

	if (m_isDesignTime)
	{
		this->m_designTimeDirty = true;
	}
}

void Level::Initialize(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* brushRegistry)
{	
	auto resources = deviceResources->GetD3DDevice();
	auto device = deviceResources->GetD2DDeviceContext();
	auto loader = ref new BasicLoader(resources);
	loader->LoadPngToBitmap(L"Media\\Bitmaps\\universe_seamless.png", deviceResources, &this->m_floorBackground);

	DX::ThrowIfFailed(device->CreateCompatibleRenderTarget(D2D1::SizeF(1000.0f, 600.0f), &this->m_dingSheet));
	DX::ThrowIfFailed(device->CreateCompatibleRenderTarget(D2D1::SizeF(blooDot::Consts::SQUARE_WIDTH, blooDot::Consts::SQUARE_HEIGHT), &this->m_dingImage));

	DX::ThrowIfFailed(device->CreateCompatibleRenderTarget(D2D1::SizeF(
		blooDot::Consts::SQUARE_WIDTH * blooDot::Consts::MAX_MOB_BLOCKS * OrientabilityIndexRotatory::NumberOfSectors,
		600.0f
	), &this->m_mobsSheet));

	DX::ThrowIfFailed(device->CreateCompatibleRenderTarget(D2D1::SizeF(
		blooDot::Consts::SQUARE_WIDTH * blooDot::Consts::MAX_MOB_BLOCKS,
		blooDot::Consts::SQUARE_HEIGHT * blooDot::Consts::MAX_MOB_BLOCKS
	), &this->m_mobsImage));

	this->m_dingSheet->BeginDraw();
	this->RegisterDing(std::make_shared<Mauer>(deviceResources, brushRegistry),					00,  1);
	this->RegisterDing(std::make_shared<Wasser>(deviceResources, brushRegistry),				01,  0);
	this->RegisterDing(std::make_shared<HighGrass>(deviceResources, brushRegistry),				02,  0);
	this->RegisterDing(std::make_shared<Snow>(deviceResources, brushRegistry),					03,  0);
	this->RegisterDing(std::make_shared<FloorStoneTile>(deviceResources, brushRegistry),		04,  0);
	this->RegisterDing(std::make_shared<Coin>(deviceResources, brushRegistry),					05,  0);
	this->RegisterDing(std::make_shared<Chest>(deviceResources, brushRegistry),					06,  0);
	this->RegisterDing(std::make_shared<SilverChest>(deviceResources, brushRegistry),			10,  0);
	this->RegisterDing(std::make_shared<GoldChest>(deviceResources, brushRegistry),				16,  3);
	this->RegisterDing(std::make_shared<Rail>(deviceResources, brushRegistry),					14,  0);
	this->RegisterDing(std::make_shared<CrackedMauer>(deviceResources, brushRegistry),			07,  1);
	this->RegisterDing(std::make_shared<FloorRockTile>(deviceResources, brushRegistry),			14,  1);
	this->RegisterDing(std::make_shared<FloorRockTileCracked>(deviceResources, brushRegistry),	15,  1);
	this->RegisterDing(std::make_shared<FloorStoneTilePurple>(deviceResources, brushRegistry),	16,  1);
	this->RegisterDing(std::make_shared<FloorStoneTileOchre>(deviceResources, brushRegistry),	17,  1);
	this->RegisterDing(std::make_shared<FloorStoneTileSlate>(deviceResources, brushRegistry),	18,  1);
	this->RegisterDing(std::make_shared<Lettuce>(deviceResources, brushRegistry),				14,  2);
	this->RegisterDing(std::make_shared<BarrelWooden>(deviceResources, brushRegistry),			15,  2);
	this->RegisterDing(std::make_shared<BarrelIndigo>(deviceResources, brushRegistry),			16,  2);
	this->RegisterDing(std::make_shared<LooseMauer>(deviceResources, brushRegistry),			14,  3);
	this->RegisterDing(std::make_shared<Door>(deviceResources, brushRegistry),					14,  4);
	DX::ThrowIfFailed(this->m_dingSheet->EndDraw());
	this->m_dingSheet->GetBitmap(&this->m_dingSheetBmp);

	this->m_mobsSheet->BeginDraw();
	this->RegisterDing(std::make_shared<Player1>(deviceResources, brushRegistry),		00,  0);
	this->RegisterDing(std::make_shared<Dalek>(deviceResources, brushRegistry),			00,  1);
	this->RegisterDing(std::make_shared<Schaedel>(deviceResources, brushRegistry),		00,  2);
	this->RegisterDing(std::make_shared<FlameGhost>(deviceResources, brushRegistry),	00,  3);
	DX::ThrowIfFailed(this->m_mobsSheet->EndDraw());
	this->m_mobsSheet->GetBitmap(&this->m_mobsSheetBmp);
}

bool Level::IsDesignMode()
{
	return this->m_isDesignTime;
}

void Level::RegisterDing(std::shared_ptr<Dings> dingDef, unsigned xOnSheet, unsigned yOnSheet)
{
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> ontoSheet;
	if (dingDef->IsMob())
	{
		ontoSheet = this->m_mobsSheet;
	}
	else
	{
		ontoSheet = this->m_dingSheet;
	}

	dingDef->Draw(ontoSheet, xOnSheet, yOnSheet);
	this->m_dingMap.emplace(dingDef->ID(), dingDef);
}

std::shared_ptr<Dings> Level::GetDing(Dings::DingIDs dingID)
{
	return this->m_dingMap.at(dingID);
}

Dings::DingIDs Level::GetPreviousDingID(Dings::DingIDs dingID)
{
	auto dingPointer = this->m_dingMap.find(dingID);
	if (dingPointer == this->m_dingMap.end())
	{
		return Dings::DingIDs::Void;
	}
	else
	{
		if (dingPointer == this->m_dingMap.begin())
		{
			return this->m_dingMap.rbegin()->second->ID();
		}
		else
		{
			return (--dingPointer)->second->ID();
		}
	}
}

Dings::DingIDs Level::GetNextDingID(Dings::DingIDs dingID)
{
	auto dingPointer = this->m_dingMap.find(dingID);
	if (dingPointer == this->m_dingMap.end())
	{
		return Dings::DingIDs::Void;
	}
	else
	{
		++dingPointer;
		if (dingPointer == this->m_dingMap.end())
		{
			return m_dingMap.begin()->second->ID();
		}
		else
		{
			return dingPointer->second->ID();
		}
	}
}

Dings::DingIDs Level::ConfirmDingID(Dings::DingIDs dingID)
{
	return this->m_dingMap.count(dingID) ? dingID : Dings::DingIDs::Void;
}

Microsoft::WRL::ComPtr<ID2D1Bitmap> Level::CreateDingImage(Dings::DingIDs dingID, Facings placementOrientation)
{
	auto dingToRender = this->GetDing(dingID);
	auto dingOnSheet = dingToRender->GetSheetPlacement(placementOrientation);
	ID2D1Bitmap *resultBitmap = NULL;
	this->m_dingImage->BeginDraw();
	this->m_dingImage->Clear();
	D2D1_RECT_F dingRect = D2D1::RectF(dingOnSheet.x * 49.0f, dingOnSheet.y * 49.0f, dingOnSheet.x * 49.0f + 49.0f, dingOnSheet.y * 49.0f + 49.0f);
	D2D1_RECT_F placementRect = D2D1::RectF(0, 0, blooDot::Consts::SQUARE_WIDTH, blooDot::Consts::SQUARE_HEIGHT);
	this->m_dingImage->DrawBitmap(this->m_dingSheetBmp.Get(), placementRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, dingRect);
	DX::ThrowIfFailed(this->m_dingImage->EndDraw());
	this->m_dingImage->GetBitmap(&resultBitmap);
	Microsoft::WRL::ComPtr<ID2D1Bitmap> resultPointer = resultBitmap;
	return resultPointer;
}

Microsoft::WRL::ComPtr<ID2D1Bitmap> Level::CreateMobImage(Dings::DingIDs dingID, Facings placementOrientation)
{
	auto dingToRender = this->GetDing(dingID);
	auto dingOnSheet = dingToRender->GetSheetPlacement(placementOrientation);
	ID2D1Bitmap *resultBitmap = NULL;
	this->m_mobsImage->BeginDraw();
	this->m_mobsImage->Clear();
	auto sheetExtent = dingToRender->GetExtentOnSheet();
	D2D1_RECT_F dingRect = D2D1::RectF(
		dingOnSheet.x * blooDot::Consts::SQUARE_WIDTH,
		dingOnSheet.y * blooDot::Consts::SQUARE_HEIGHT,
		dingOnSheet.x * blooDot::Consts::SQUARE_WIDTH + (sheetExtent.width * blooDot::Consts::SQUARE_WIDTH),
		dingOnSheet.y * blooDot::Consts::SQUARE_HEIGHT + (sheetExtent.height * blooDot::Consts::SQUARE_HEIGHT)
	);

	D2D1_RECT_F placementRect = D2D1::RectF(
		0,
		0,
		sheetExtent.width * blooDot::Consts::SQUARE_WIDTH,
		sheetExtent.height * blooDot::Consts::SQUARE_HEIGHT
	);

	this->m_mobsImage->DrawBitmap(this->m_mobsSheetBmp.Get(), placementRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, dingRect);

	
	DX::ThrowIfFailed(this->m_mobsImage->EndDraw());
	this->m_mobsImage->GetBitmap(&resultBitmap);
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

Blocks* Level::GetBlocksAt(int levelX, int levelY, bool createIfNull)
{
	auto objectAddress = levelY * this->m_rectangularBounds.width + levelX;
	auto retrievedObject = (Blocks*)nullptr;

	/* we create lazily, on demand */
	if (objectAddress >= 0 && levelX > 0 && objectAddress < this->m_Blocks.size())
	{
		retrievedObject = this->m_Blocks[objectAddress];
		if (retrievedObject == nullptr && createIfNull)
		{
			auto newObject = new Blocks(levelX, levelY);
			newObject->PlaceInLevel(this->shared_from_this());
			this->m_Blocks[objectAddress] = newObject;
			retrievedObject = newObject;
			if (this->m_isDesignTime)
			{
				this->m_designTimeDirty = true;
			}
		}
	}

	return retrievedObject;
}

std::shared_ptr<Dings> Level::WeedObjectAt(unsigned levelX, unsigned levelY, Layers* cullCoalescableInLayer)
{
	std::shared_ptr<Dings> dingWeeded = nullptr;
	(*cullCoalescableInLayer) = Layers::None;
	auto objectAddress = levelY * this->m_rectangularBounds.width + levelX;
	if (objectAddress >= 0 && objectAddress < this->m_Blocks.size())
	{
		auto existingObject = this->m_Blocks[objectAddress];
		if (existingObject != nullptr)
		{
			auto weedComplete = existingObject->WeedFromTop(&dingWeeded, cullCoalescableInLayer);
			if (weedComplete)
			{
				this->m_Blocks[objectAddress] = nullptr;
			}

			if (dingWeeded != nullptr && this->m_isDesignTime)
			{
				this->m_designTimeDirty = true;
			}
		}
	}

	return dingWeeded;
}

void Level::SetupRuntimeState()
{
	for (unsigned y = 0; y < this->m_rectangularBounds.height; ++y)
	{
		for (unsigned x = 0; x < this->m_rectangularBounds.width; ++x)
		{
			auto objectAddress = y * this->m_rectangularBounds.width + x;
			if (objectAddress >= 0 && objectAddress < this->m_Blocks.size())
			{
				auto allocatedObject = this->m_Blocks[objectAddress];
				if (allocatedObject != nullptr)
				{					
					auto layers = allocatedObject->GetLayers();
					if (layers & Layers::Floor)
					{
						allocatedObject->GetObject(Layers::Floor)->SetupRuntimeState();
					}

					if (layers & Layers::Walls)
					{
						allocatedObject->GetObject(Layers::Walls)->SetupRuntimeState();
					}

					if (layers & Layers::Rooof)
					{
						allocatedObject->GetObject(Layers::Rooof)->SetupRuntimeState();
					}
				}
			}
		}
	}
}

ClumsyPacking::NeighborConfiguration Level::GetNeighborConfigurationOf(unsigned levelX, unsigned levelY, Dings::DingIDs dingID, Layers inLayer)
{
	if (dingID > Dings::DingIDs::Void)
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

bool Level::HasCompatibleNeighbor(int x, int y, Dings::DingIDs dingID, Layers ofLayer)
{
	if (!(x < 0 || y < 0 || x >= (int)this->m_rectangularBounds.width || y >= (int)this->m_rectangularBounds.height))
	{
		auto thereObject = this->GetBlocksAt(x, y, false);
		if (thereObject != nullptr && ((thereObject->GetLayers() & ofLayer) == ofLayer))
		{
			auto thereDing = thereObject->GetObject(ofLayer)->GetDing();
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

Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> Level::GetMobsSheet()
{
	return this->m_mobsSheet;
}

ID2D1Bitmap* Level::GetDingSheetBmp()
{
	return this->m_dingSheetBmp.Get();
}

ID2D1Bitmap* Level::GetMobsSheetBmp()
{
	return this->m_mobsSheetBmp.Get();
}

unsigned Level::GetNumOfSheetsRequired(unsigned extentUnits, unsigned sizePerSheet)
{
	return static_cast<unsigned>(ceilf(static_cast<float>(extentUnits) / static_cast<float>(sizePerSheet)));
}

Microsoft::WRL::ComPtr<ID2D1Bitmap> Level::GetFloorBackground()
{
	return this->m_floorBackground;
}

D2D1_RECT_U Level::DeterminePopulatedAreaBounds()
{
	auto outerBounds = D2D1::RectU(UINT32_MAX, UINT32_MAX, 0, 0);
	for (unsigned y = 0; y < this->m_rectangularBounds.height; ++y)
	{
		for (unsigned x = 0; x < this->m_rectangularBounds.width; ++x)
		{
			auto objectAddress = y * this->m_rectangularBounds.width + x;
			if (objectAddress >= 0 && objectAddress < this->m_Blocks.size())
			{
				auto existingObject = this->m_Blocks[objectAddress];
				if (existingObject != nullptr && existingObject->GetLayers() > Layers::None && existingObject->GetLayers()!=Layers::Rooof)
				{
					if (outerBounds.left > x)
					{
						outerBounds.left = x;
					}

					if (outerBounds.top > y)
					{
						outerBounds.top = y;
					}

					if (outerBounds.right < x)
					{
						outerBounds.right = x;
					}

					if (outerBounds.bottom < y)
					{
						outerBounds.bottom = y;
					}
				}
			}
		}
	}

	return outerBounds;
}

void Level::SetDesignTime()
{
	this->m_isDesignTime = true;
}

bool Level::HasSaveFileNameBeenSpecifiedBefore()
{
	return !this->m_lastSavedAsFileName->IsEmpty();
}

void Level::DesignSaveToFile(Platform::String^ fileName)
{
	const unsigned char levelFileFormatVersion = 2;

	unsigned blockSaveCount = 0;
	unsigned char whatsthere;
	std::wstring fName = fileName->Data();
	std::ofstream oF;
	oF.open(fName, ios_base::out | ios_base::binary);
	/* header */
	oF.write((char*)&blooDot::blooDotMain::BLOODOTFILE_SIGNATURE, sizeof(blooDot::blooDotMain::BLOODOTFILE_SIGNATURE));
	oF.write((char*)&sigbyte, sizeof(unsigned char));
	oF.write((char*)&blooDot::blooDotMain::BLOODOTFILE_CONTENTTYPE_LEVEL_DESIGN, sizeof(blooDot::blooDotMain::BLOODOTFILE_CONTENTTYPE_LEVEL_DESIGN));
	oF.write((char*)&levelFileFormatVersion, sizeof(unsigned char));
	/* dimensions */
	oF.write((char*)&m_rectangularBounds.height, sizeof(uint32));
	oF.write((char*)&m_rectangularBounds.width, sizeof(uint32));
	/* skip table */
	for (unsigned y = 0; y < this->m_rectangularBounds.height; ++y)
	{
		for (unsigned x = 0; x < this->m_rectangularBounds.width; ++x)
		{
			auto objectAddress = y * this->m_rectangularBounds.width + x;
			if (objectAddress >= 0 && objectAddress < this->m_Blocks.size())
			{
				auto allocatedObject = this->m_Blocks[objectAddress];
				if (allocatedObject != nullptr)
				{
					whatsthere = 0x0;
					auto layers = allocatedObject->GetLayers();
					if (layers & Layers::Floor)
					{
						whatsthere |= floorbit;
					}

					if (layers & Layers::Walls)
					{
						whatsthere |= wallsbit;
					}

					if (layers & Layers::Rooof)
					{
						whatsthere |= rooofbit;
					}

					if (whatsthere > 0)
					{
						/* the square address descriptor is 24bit, so compose it accordingly */
						uint64 addressDescriptor = ((y & 0x3ff) << 14) | (((whatsthere << 1) + 1) << 10) | (x & 0x3ff);
						byte msb = (addressDescriptor >> 16) & 0xff;
						byte isb = (addressDescriptor >> 8) & 0xff;
						byte lsb = addressDescriptor & 0xff;
						oF.write((char *)&msb, sizeof(unsigned char));
						oF.write((char *)&isb, sizeof(unsigned char));
						oF.write((char *)&lsb, sizeof(unsigned char));
						if (layers & ::Floor)
						{
							allocatedObject->DesignSaveToFile(&oF, ::Floor);
						}

						if (layers & ::Walls)
						{
							allocatedObject->DesignSaveToFile(&oF, ::Walls);
						}

						if (layers & ::Rooof)
						{
							allocatedObject->DesignSaveToFile(&oF, ::Rooof);
						}
#ifdef _DEBUG
						++blockSaveCount;
#endif
					}
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

	this->m_designTimeDirty = false;
	this->m_lastSavedAsFileName = fileName;
}

bool Level::DesignLoadFromFile(Platform::String^ fileName)
{
	BasicReaderWriter^ basicReaderWriter;
	size_t offset = 0L;
	std::ifstream iF;
	iF.open(fileName->Data(), ios_base::in | ios_base::binary);
	if (!iF.fail() && iF.is_open())
	{
		iF.seekg(0, std::ios::end);
		auto length = iF.tellg();
		iF.seekg(0, std::ios::beg);
		char* srcData = new char[length];
		iF.read(srcData, length);

		/* parse the signature */
		uint64 fileSignature = *reinterpret_cast<uint64*>(srcData + offset); offset += sizeof(uint64);
		uint64 signatToMatch = (uint64_t)(*(uint64_t*)&blooDot::blooDotMain::BLOODOTFILE_SIGNATURE[0]);
		if (fileSignature != signatToMatch)
		{
			return false;
		}

		unsigned char sigByte = *reinterpret_cast<unsigned char*>(srcData + offset); offset += sizeof(unsigned char);
		if (sigByte != Level::sigbyte)
		{
			return false;
		}

		uint16 fileType = *reinterpret_cast<uint16*>(srcData + offset); offset += sizeof(uint16);
		uint16 tToMatch = (uint16_t)(*(uint16_t*)&blooDot::blooDotMain::BLOODOTFILE_CONTENTTYPE_LEVEL_DESIGN[0]);
		if (fileType != tToMatch)
		{
			return false;
		}

		/* parse the actual descriptor content, may differ depending on version */
		unsigned versionByte = static_cast<unsigned>(*reinterpret_cast<byte*>(srcData + offset)); offset += sizeof(byte);
		switch (versionByte)
		{
			case 1:
				throw ref new Platform::FailureException(L"deprecated level design file version");

			case 2:
				this->DesignLoadFromFile_version2(srcData, length, offset);
				break;

			default:
				throw ref new Platform::FailureException(L"invalid level design file version");
		}

		delete[] srcData;
		/* so next time hitting "Save" will not prompt for file name */
		this->m_lastSavedAsFileName = fileName;
		this->m_designTimeDirty = false;
		return true;
	}
	else
	{
		return false;
	}
}

void Level::DesignLoadFromFile_version2(char* srcData, const size_t length, size_t offset)
{
#ifdef _DEBUG
	unsigned blockLoadCount = 0;
#endif	

	/* read and apply the size */
	unsigned short extentY = *reinterpret_cast<uint32*>(srcData + offset); offset += sizeof(uint32);
	unsigned short extentX = *reinterpret_cast<uint32*>(srcData + offset); offset += sizeof(uint32);
	this->m_rectangularBounds.width = extentX;
	this->m_rectangularBounds.height = extentY;
	this->Clear();
	try 
	{
		/* read the descriptor stream */
		while (offset < (length - 1))
		{
			byte msb = *reinterpret_cast<byte*>(srcData + offset); offset += sizeof(const byte);
			byte isb = *reinterpret_cast<byte*>(srcData + offset); offset += sizeof(const byte);
			byte lsb = *reinterpret_cast<byte*>(srcData + offset); offset += sizeof(const byte);
			int32 placementDescriptor = (msb << 16 | isb << 8 | lsb) & 0xffffff;
			uint32 coordinateY = (placementDescriptor >> 14) & 0x3ff;
			byte layerFlags = (placementDescriptor >> 11) & 0x7;
			uint32 coordinateX = placementDescriptor & 0x3ff;
			bool
				hasFloor = layerFlags & Level::floorbit,
				hasWalls = layerFlags & Level::wallsbit,
				hasRooof = layerFlags & Level::rooofbit;

			if (hasFloor)
			{
				hasFloor = this->CellLoadFromFile(srcData, &offset, Layers::Floor, coordinateX, coordinateY);
			}

			if (hasWalls)
			{
				hasWalls = this->CellLoadFromFile(srcData, &offset, Layers::Walls, coordinateX, coordinateY);
			}

			if (hasRooof)
			{
				hasRooof = this->CellLoadFromFile(srcData, &offset, Layers::Rooof, coordinateX, coordinateY);
			}

#ifdef _DEBUG
			blockLoadCount += static_cast<unsigned>(hasRooof) + static_cast<unsigned>(hasWalls) + static_cast<unsigned>(hasFloor);
#endif
		}
	}
	catch (const std::exception& ex)
	{
		blooDot::Utility::DebugOutputException(ex);
	}

#ifdef _DEBUG
	const int bufferLength = 16;
	char16 str[bufferLength];
	int len = swprintf_s(str, bufferLength, L"%d", blockLoadCount);
	Platform::String^ string = ref new Platform::String(str, len);
	OutputDebugStringW(Platform::String::Concat(Platform::String::Concat(L"Number of non-empty squares loaded: ", string), L"\r\n")->Data());
#endif
}

bool Level::CellLoadFromFile(char *srcData, size_t *offset, const Layers inLayer, const uint32 coordinateX, const uint32 coordinateY)
{
	Facings placementFacing;
	byte objectDescriptor = *reinterpret_cast<byte*>(srcData + (*offset)); (*offset) += sizeof(const byte);
	auto dingID = static_cast<Dings::DingIDs>(objectDescriptor & 0x3f);
	if (objectDescriptor & 0x40)
	{
		/* yes, this format is a bitch. but it is oh so efficient,
		 * and it contains all of the retro computing flair */
		byte secondDingByte = *reinterpret_cast<byte*>(srcData + (*offset)); (*offset) += sizeof(const byte);
		dingID = static_cast<Dings::DingIDs>((secondDingByte << 6) | static_cast<unsigned>(dingID));
	}
	
	if (dingID > Dings::DingIDs::Void && this->m_dingMap.find(dingID) != this->m_dingMap.end())
	{
		if (objectDescriptor & 0x80)
		{
			placementFacing = static_cast<Facings>(*reinterpret_cast<unsigned int*>(srcData + (*offset))); (*offset) += sizeof(const unsigned int);
		}
		else
		{
			placementFacing = Facings::Shy;
		}

		this->GetBlocksAt(coordinateX, coordinateY, true)->InstantiateInLayerFacing(this->shared_from_this(), inLayer, this->GetDing(dingID), placementFacing);
		return true;
	}

	return false;
}