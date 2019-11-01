#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\io\BasicLoader.h"
#include "..\dx\BrushRegistry.h"
#include "..\dings\dings.h"

/* A level is a set of worldsheets belonging to the same floor level, hence its name */
class Level
{
public:
	Level::Level(Platform::String^ levelName, D2D1_SIZE_U sheetSize, unsigned extentWE, unsigned extentNS);
	D2D1_SIZE_U GetRectBoundsUnits();
	D2D1_SIZE_U GetSheetSizeUnits();
	unsigned GetNumOfSheetsWE();
	unsigned GetNumOfSheetsNS();

private:
	unsigned GetNumOfSheetsRequired(unsigned extentUnits, unsigned sizePerSheet);

	D2D1_SIZE_U					m_rectangularBounds;
	D2D1_SIZE_U					m_sheetSize;
};