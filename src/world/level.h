#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\io\BasicLoader.h"
#include "..\dx\BrushRegistry.h"
#include "..\dings\dings.h"
#include "Object.h"

/* A level is a set of worldsheets belonging to the same floor level, hence its name */
class Level
{
public:
	Level::Level(Platform::String^ levelName, D2D1_SIZE_U sheetSize, unsigned extentWE, unsigned extentNS);
	Level::~Level();

	D2D1_SIZE_U GetRectBoundsUnits();
	D2D1_SIZE_U GetSheetSizeUnits();
	unsigned GetNumOfSheetsWE();
	unsigned GetNumOfSheetsNS();
	Object* GetObjectAt(unsigned levelX, unsigned levelY);
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> GetDingSheet();

private:
	unsigned GetNumOfSheetsRequired(unsigned extentUnits, unsigned sizePerSheet);

	Platform::String^									m_Name;
	D2D1_SIZE_U											m_rectangularBounds;
	D2D1_SIZE_U											m_sheetSize;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget>		m_dingSheet;
	std::vector<Object*>								m_Objects;
};