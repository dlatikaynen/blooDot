#pragma once

#include "..\PreCompiledHeaders.h"
#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\dx\BrushRegistry.h"
#include "..\io\BasicLoader.h"

// Numeric ordering is relied upon
enum Layers
{
	None = 0,
	Floor = 1,
	Walls = 2,
	Rooof = 4
};

enum Facings
{
	Shy = 0,													/* one shy (default placement) */
	NW = 1,														/* left-top capping edge */
	North = 2,													/* U-bag with opening downwards (northcap) */
	NE = 4,														/* right-top capping edge */
	East = 8,													/* U-bag with opening to the left (eastcap) */
	SE = 16,													/* right-bottom capping edge */
	South = 32,													/* U-bag with opening upwards (southcap) */
	SW = 64,													/* left-bottom capping edge */
	West = 128,													/* U-bag with opening to the right (westcap) */
	Viech = West | North | East | South,						/* in fact anything that can orient left-right-up-down, all viech mobs and stuff like chests */
	Center = 256,												/* crossing */
	SingleEdge = 512,
	TripleEdge = 1024,
	CornerNear = 2048,
	CornerFar = 4096,
	WE = West | East,											/* horizontal pipe */
	NS = North | South,											/* vertical pipe */
	EdgeW = West | SingleEdge,									/* straight edge, left-only */
	EdgeE = East | SingleEdge,									/* straight edge, right-only */
	EdgeN = North | SingleEdge,									/* straight edge, up-only */
	EdgeS = South | SingleEdge,									/* straight edge, down-only */
	EdgeNW = North  | West | TripleEdge,						/* left upper outer edge */
	EdgeNE = North | East | TripleEdge,							/* right upper outer edge */
	EdgeSW = South | West | TripleEdge,							/* bottom left outer edge */
	EdgeSE = South | East | TripleEdge,							/* bottom right outer edge */
	EdgeCornerWD = West | SingleEdge | CornerNear,				/* left edge with one lower inner corner */
	EdgeCornerWU = West | SingleEdge | CornerFar,				/* left edge with one upper inner corner */
	EdgeCornerED = East | SingleEdge | CornerNear,				/* right edge with one lower inner corner */
	EdgeCornerEU = East | SingleEdge | CornerFar,				/* right edge with one upper inner corner */
	EdgeCornerSL = South | SingleEdge | CornerNear,				/* lower edge with one left inner corner */
	EdgeCornerSR = South | SingleEdge | CornerFar,				/* lower edge with one right inner corner */
	EdgeCornerNL = North | SingleEdge | CornerNear,				/* upper edge with one left inner corner */
	EdgeCornerNR = North | SingleEdge | CornerFar,				/* upper edge with one right inner corner */
	EdgeCornerNW = North | West | TripleEdge | CornerFar,		/* left upper edge with opposite inner corner */
	EdgeCornerNE = North | East | TripleEdge | CornerFar,		/* right upper with opposite inner corner */
	EdgeCornerSW = South | West | TripleEdge | CornerFar,		/* left bottom edge with opposite inner inner corner */
	EdgeCornerSE = South | East | TripleEdge | CornerFar,		/* right bottom edge with opposite inner corner */
	TW = West | SingleEdge | CornerNear | CornerFar,			/* T facing left */
	TE = East | SingleEdge | CornerNear | CornerFar,			/* T facing right */
	TS = South | SingleEdge | CornerNear | CornerFar,			/* T facing down */
	TN = North | SingleEdge | CornerNear | CornerFar,			/* T facing up */
	Corner1NW = North | West | CornerNear,						/* single inner top left corner */
	Corner1NE = North | East | CornerNear,						/* single inner top right corner */
	Corner1SW = South | West | CornerNear,						/* single inner bottom left corner */
	Corner1SE = South | East | CornerNear,						/* single inner bottom right corner */
	Corner2W = West | CornerNear | CornerFar,					/* double corners left side */
	Corner2E = East | CornerNear | CornerFar,					/* double corners right side */
	Corner2S = South | CornerNear | CornerFar,					/* double corners down */
	Corner2N = North | CornerNear | CornerFar,					/* double corners top */
	CornerNWSE = North | West | CornerNear | CornerFar,			/* diagonal double corner top-left / right-bottom */
	CornerSWNE = South | East | CornerNear | CornerFar,			/* diagnoal double corner bottom-left / right-top */
	Corner3NW = North | West | CornerFar,						/* triple inner corner pointing top left */
	Corner3NE = North | East | CornerFar,						/* triple inner corner pointing top right */
	Corner3SW = South | West | CornerFar,						/* triple inner corner pointing bottom left */
	Corner3SE = South | East | CornerFar,						/* triple inner corner pointing bottom right */
	Immersed = NW | North | NE | East | SE | South | SW | West
};

constexpr inline Facings operator |(const Facings left, const Facings& right) 
{
	return static_cast<Facings>(static_cast<int>(left) | static_cast<int>(right));
}

Facings& operator |=(Facings& a, Facings b);

enum OrientabilityIndexQuadruplet
{
	// Sequence is meaningful
	// and must remain so we can
	// say like, i_orientability * 90
	// for rotation in a loop
	Lefty = 0,
	Downy = 1,
	Righty = 2,
	Uppy = 3,
	LeftyFar = 4,
	DownyFar = 5,
	RightyFar = 6,
	UppyFar = 7
};

const int FAR_OFFSET = 4;

enum OrientabilityIndexDiagon
{
	DiagNW = 0,
	DiagSW = 1,
	DiagSE = 2,
	DiagNE = 3
};

enum OrientabilityIndexDuplex
{
	Horizontally = 0,
	Vertically = 1
};

// A Ding Wang Tile descriptor, and a candidate for clumsy packing on a 2-topology
class Dings
{
public:
	Dings(int dingID, Platform::String^ dingName, std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);

	unsigned			ID();
	Platform::String^	Name();
	void				Draw(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, int canvasX, int canvasY);
	D2D1_POINT_2U		GetSheetPlacement(Facings orientation);
	Layers				GetPreferredLayer();
	Facings				AvailableFacings();
	bool				CouldCoalesce();
	static Facings		RotateFromFacing(Facings fromFacing, bool inverseDirection);

protected:
	unsigned			m_ID;
	Platform::String^	m_Name;
	BrushRegistry*		m_Brushes;
	Facings				m_Facings;
	Facings				m_Coalescing;
	Layers				m_preferredLayer;
	Layers				m_possibleLayers;
	
	/* squareish dings come in sets of 
	 * topologically predictable coalescability, thusly:
	 *
	 * one shy (default placement)
	 * one immersed
	 * one crossing
	 * two pipes
	 * two diagonal inner corners
	 * four single inner corners
	 * four double inner corners
	 * four triple inner corners
	 * four outer-inner 90° corners
	 * four outer-only 90° corners
	 * four U-pieces
	 * four T-pieces
	 * four straight edges
	 * four straight edges with one inner corner
	 * four straight edges with two inner corners (Ts) 
	 *
	 * 10 x 4 + 2 x 2 + 3 x 1 = 40 + 4 + 3 = 47, the Wang number
	 * so we seem to have it somewhat right, exactly, that is.
	 *
	 * indices are always orientationwise */

	D2D1_POINT_2U		m_lookupShy;
	D2D1_POINT_2U		m_lookupImmersed;
	D2D1_POINT_2U		m_lookupCrossing;
	D2D1_POINT_2U		m_lookupPipes[2];
	D2D1_POINT_2U		m_lookupCornersDiag[2];
	D2D1_POINT_2U		m_lookupCornersInner1[4];
	D2D1_POINT_2U		m_lookupCornersInner2[4];
	D2D1_POINT_2U		m_lookupCornersInner3[4];
	D2D1_POINT_2U		m_lookupCornersBoth90[4];
	D2D1_POINT_2U		m_lookupEdgesOuter90[4];
	D2D1_POINT_2U		m_lookupSides[4];
	D2D1_POINT_2U		m_lookupEdges[4];
	D2D1_POINT_2U		m_lookupEdgesInner1[8];
	D2D1_POINT_2U		m_lookupU[4];
	D2D1_POINT_2U		m_lookupTs[4];

	virtual Platform::String^ ShouldLoadFromBitmap();
	void PrepareRect(D2D1_POINT_2U *lookupLocation, D2D1_RECT_F &rectToSet);
	void PrepareRect7x7(D2D1_POINT_2U *lookupLocation, D2D1_RECT_F &rectToSet);
	void Rotate(ID2D1RenderTarget *rendEr, D2D1_RECT_F rect, int rotation);
	Microsoft::WRL::ComPtr<ID2D1Bitmap> LoadBitmap(Platform::String^ fileName);
	virtual void PrepareBackground(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) { };
	virtual void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo);
	virtual void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, D2D1_RECT_F rect);

private:
	void SetSheetPlacementsFromCoalescability();
	Microsoft::WRL::ComPtr<ID2D1Bitmap> LoadFromBitmap();
	void Pack7x7(unsigned offsetX, unsigned offsetY, unsigned* x, unsigned* y);
	unsigned Pack7x7X(unsigned offsetX, unsigned offsetY, unsigned* x, unsigned* y);
	unsigned Pack7x7Y(unsigned offsetX, unsigned offsetY, unsigned* x, unsigned* y);
	void DrawShy(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo);
	void DrawTwin(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo);
	void DrawQuadruplet(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo);
	void DrawClumsyPack(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo);

	std::shared_ptr<DX::DeviceResources> m_deviceResources;
	Platform::String^ m_fromFile;
};

class Mauer : public Dings 
{
public:
	Mauer(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
protected:
	void Mauer::PrepareBackground(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
};

class CrackedMauer : public Mauer
{
public:
	CrackedMauer(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
protected:
	void PrepareBackground(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
};

class Wasser : public Dings
{
public:
	Wasser(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
};

class HighGrass : public Dings
{
public:
	HighGrass(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Snow : public Dings
{
public:
	Snow(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Dalek : public Dings
{
public:
	Dalek(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class FloorStoneTile : public Dings
{
public:
	FloorStoneTile(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Coin : public Dings
{
public:
	Coin(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
};

class Chest : public Dings
{
public:
	Chest(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, D2D1_RECT_F rect) override;
};

class Rail : public Dings
{
public:
	Rail(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, D2D1_RECT_F rect) override;
};
