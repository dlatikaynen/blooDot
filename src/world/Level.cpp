#include "..\PreCompiledHeaders.h"
#include "Level.h"

Level::Level(Platform::String^ levelName, D2D1_SIZE_U sheetSize, unsigned extentWE, unsigned extentNS)
{
	this->m_rectangularBounds = D2D1::SizeU(extentWE, extentNS);
	this->m_sheetSize = sheetSize;
	this->m_Name = levelName;

	/* generates the matrix for this level, lazy-loading */
	for (auto y = 0; y < extentNS; ++y)
	{
		for (auto x = 0; x < extentWE; ++x)
		{					
			this->m_Objects.push_back((Object*)nullptr);
		}
	}
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
	auto objectAddress = levelY * m_rectangularBounds.width + levelX;
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
