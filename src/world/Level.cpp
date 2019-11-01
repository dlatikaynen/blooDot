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

Object* Level::GetObjectAt(unsigned levelX, unsigned levelY)
{
	auto objectAddress = levelY * m_rectangularBounds.width + levelX;

	/* we create lazily, on demand */
	auto retrievedObject = this->m_Objects[objectAddress];
	if (retrievedObject == nullptr) 
	{
		auto newObject = new Object(levelX, levelY);
		this->m_Objects[objectAddress] = newObject;
		retrievedObject = newObject;
	}

	return retrievedObject;
}

unsigned Level::GetNumOfSheetsRequired(unsigned extentUnits, unsigned sizePerSheet)
{
	return static_cast<unsigned>(ceilf(static_cast<float>(extentUnits) / static_cast<float>(sizePerSheet)));
}
