#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\io\BasicLoader.h"
#include "..\dx\BrushRegistry.h"
#include "..\dings\dings.h"
#include "..\algo\ClumsyPacking.h"
#include "Object.h"

/* A level is a set of worldsheets belonging to the same floor level, hence its name :) */
class Level : public std::enable_shared_from_this<Level>
{
public:
	Level::Level(Platform::String^ levelName, D2D1_SIZE_U sheetSize, unsigned extentWE, unsigned extentNS);

	Level::Level(const Object& obj) { };
	Level& operator=(const Level& obj) { return *this; };
	Level::Level() { };
	Level::Level(Level&& obj) { };
	Level::~Level();

	void Initialize(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* brushRegistry);
	bool IsDesignMode();
	D2D1_SIZE_U GetRectBoundsUnits();
	D2D1_SIZE_U GetSheetSizeUnits();
	unsigned GetNumOfSheetsWE();
	unsigned GetNumOfSheetsNS();
	Object* GetObjectAt(int levelX, int levelY, bool createIfNull);
	ClumsyPacking::NeighborConfiguration GetNeighborConfigurationOf(unsigned levelX, unsigned levelY, Dings::DingIDs dingID, Layers inLayer);
	std::shared_ptr<Dings> WeedObjectAt(unsigned levelX, unsigned levelY, Layers* cullCoalescableInLayer);
	void SetupRuntimeState();
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> GetDingSheet();
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> GetMobsSheet();
	ID2D1Bitmap* GetDingSheetBmp();
	ID2D1Bitmap* GetMobsSheetBmp();
	std::shared_ptr<Dings> GetDing(Dings::DingIDs dingID);
	Dings::DingIDs GetNextDingID(Dings::DingIDs dingID);
	Dings::DingIDs GetPreviousDingID(Dings::DingIDs dingID);
	Dings::DingIDs ConfirmDingID(Dings::DingIDs dingID);
	Microsoft::WRL::ComPtr<ID2D1Bitmap> CreateDingImage(Dings::DingIDs dingID, Facings placementOrientation);
	Microsoft::WRL::ComPtr<ID2D1Bitmap> CreateMobImage(Dings::DingIDs dingID, Facings placementOrientation);
	Microsoft::WRL::ComPtr<ID2D1Bitmap> GetFloorBackground();
	D2D1_RECT_U DeterminePopulatedAreaBounds();

	void SetDesignTime();
	bool DesignLoadFromFile(Platform::String^ fileName);
	void DesignSaveToFile(Platform::String^ fileName);
	bool HasSaveFileNameBeenSpecifiedBefore();

protected:
	void RegisterDing(std::shared_ptr<Dings> dingDef, unsigned xOnSheet, unsigned yOnSheet);

private:
	const unsigned char sigbyte = 42;
	const unsigned char floorbit = 0x4;
	const unsigned char wallsbit = 0x2;
	const unsigned char rooofbit = 0x1;

	void Clear();
	unsigned GetNumOfSheetsRequired(unsigned extentUnits, unsigned sizePerSheet);
	bool HasCompatibleNeighbor(int x, int y, Dings::DingIDs dingID, Layers ofLayer);
	void DesignLoadFromFile_version2(char* srcData, const size_t length, size_t offset);
	bool CellLoadFromFile(char *srcData, size_t *offset, const Layers inLayer, const uint32 coordinateX, const uint32 coordinateY);

	Platform::String^									m_Name;
	D2D1_SIZE_U											m_rectangularBounds;
	std::map<Dings::DingIDs, const std::shared_ptr<Dings>>	m_dingMap;
	D2D1_SIZE_U											m_sheetSize;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>					m_floorBackground;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget>		m_dingSheet;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget>		m_mobsSheet;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget>		m_dingImage;
	Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget>		m_mobsImage;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>					m_dingSheetBmp;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>					m_mobsSheetBmp;

	std::vector<Object*>								m_Objects;
	bool												m_isDesignTime;
	Platform::String^									m_lastSavedAsFileName;
	bool												m_designTimeDirty;
};