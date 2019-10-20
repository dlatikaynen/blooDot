#pragma once

#include "..\PreCompiledHeaders.h"
#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\dx\BrushRegistry.h"

enum Layers
{
	Floor = 1,
	Walls = 2,
	Rooof = 4
};

enum Facings
{
	Shy = 0,
	North = 1,
	East = 2,
	South = 4,
	West = 8,
	NE = North | East,
	NS = North | South,
	FullyCoalesced = North | East | South | West
};

class Dings
{
public:
	Dings(int dingID, Platform::String^ dingName, BrushRegistry drawBrushes);

	int					ID();
	Platform::String^	Name();
	void				Draw(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, int canvasX, int canvasY);
	virtual void		DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo);
	D2D1_POINT_2U		GetSheetPlacement(Facings coalesced);

protected:
	int					m_ID;
	Platform::String^	m_Name;
	BrushRegistry		m_Brushes;
	Facings				m_Facing;
	Facings				m_Coalescing;
	Layers				m_preferredLayer;
	Layers				m_possibleLayers;
	
	D2D1_POINT_2U		m_sheetPlacement;
	D2D1_POINT_2U		m_sheetPlacementCN;
	D2D1_POINT_2U		m_sheetPlacementCS;
	D2D1_POINT_2U		m_sheetPlacementCE;
	D2D1_POINT_2U		m_sheetPlacementCW;
	D2D1_POINT_2U		m_sheetPlacementCNS;
	D2D1_POINT_2U		m_sheetPlacementCEW;
	D2D1_POINT_2U		m_sheetPlacementCNE;
	D2D1_POINT_2U		m_sheetPlacementCES;
	D2D1_POINT_2U		m_sheetPlacementCSW;
	D2D1_POINT_2U		m_sheetPlacementCWN;
	D2D1_POINT_2U		m_sheetPlacementCWNE;
	D2D1_POINT_2U		m_sheetPlacementCNES;
	D2D1_POINT_2U		m_sheetPlacementCESW;
	D2D1_POINT_2U		m_sheetPlacementCSWN;
	D2D1_POINT_2U		m_sheetPlacementFC;

private:
	void				SetSheetPlacementsFromCoalescability();
};

class Mauer : public Dings 
{
public:
	Mauer(BrushRegistry drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo);
};