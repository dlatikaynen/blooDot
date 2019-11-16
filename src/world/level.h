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

	void Initialize(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* brushRegistry);
	D2D1_SIZE_U GetRectBoundsUnits();
	D2D1_SIZE_U GetSheetSizeUnits();
	unsigned GetNumOfSheetsWE();
	unsigned GetNumOfSheetsNS();
	Object* GetObjectAt(unsigned levelX, unsigned levelY, bool createIfNull);
	bool WeedObjectAt(unsigned levelX, unsigned levelY);
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> GetDingSheet();
	Dings* GetDing(unsigned dingID);
	Microsoft::WRL::ComPtr<ID2D1Bitmap> GetFloorBackground();

private:
	unsigned GetNumOfSheetsRequired(unsigned extentUnits, unsigned sizePerSheet);

	Platform::String^									m_Name;
	D2D1_SIZE_U											m_rectangularBounds;
	std::map<unsigned, Dings>							m_dingMap;
	D2D1_SIZE_U											m_sheetSize;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>					m_floorBackground;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget>		m_dingSheet;
	std::vector<Object*>								m_Objects;
};