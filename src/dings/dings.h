#pragma once

#include "..\PreCompiledHeaders.h"
#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\dx\BrushRegistry.h"
#include "..\io\BasicLoader.h"
#include "..\world\Bounding.h"
#include "ObjectBehaviors.h"

// flagset bitfield. numeric ordering is relied upon
enum Layers
{
	None = 0,
	Floor = 1,
	Walls = 2,
	Rooof = 4
};

enum Facings : unsigned int
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
	Cross = West | North | East | South,						/* stuff like chests */
	Center = 256,												/* crossing; in a sense that there is a NS and a WE ("pipes") */
	SingleEdge = 512,
	TripleEdge = 1024,
	CornerNear = 2048,
	CornerFar = 4096,
	NNW = 8192,
	NWW = 16384,
	SSW = 32768,
	SWW = 65536,
	NEE = 131072,
	NNE = 262144,
	SEE = 524288,
	SSE = 1048576,
	Viech = SSE | SEE | NEE | NNE | SWW | SSW | SEE | SSE,		/* anything that can orient in 16 headings, all viech mobs and players */
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
	Immersed = NW | North | NE | East | SE | South | SW | West	/* non-directional, all-coalescing joker polyomino */
};

constexpr inline Facings operator |(const Facings left, const Facings& right) 
{
	return static_cast<Facings>(static_cast<int>(left) | static_cast<int>(right));
}

Facings& operator |=(Facings& a, Facings b);

constexpr inline Layers operator |(const Layers top, const Layers& bottom)
{
	return static_cast<Layers>(static_cast<int>(top) | static_cast<int>(bottom));
}

Layers& operator |=(Layers& a, Layers b);

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

enum OrientabilityIndexRotatory : int
{
	// Sequence is meaningful
	// and must remain so we increment
	// rotation with single step
	// multiple field value by 22.5 to get aviation heading in degrees
	HDG360 = 0, // aviation: north
	HDG22_5 = 1,
	HDG45= 2,
	HDG67_5 = 3,
	HDG90 = 4, // aviation: east
	HDG112_5 = 5,
	HDG135 = 6,
	HDG157_5 = 7,
	HDG180 = 8, // aviation: south
	HDG202_5 = 9,
	HDG225 = 10,
	HDG247_5= 11,
	HDG270 = 12,
	HDG292_5 = 13,
	HDG315 = 14,
	HDG337_5 = 15,
	NumberOfSectors = 16
};

// A Ding Wang Tile descriptor, and a candidate for clumsy packing on a 2-topology
class Dings
{
public:
	static const Facings DefaultMobFacing = Facings::West;
	static const float sectorScale;

	enum class DingIDs : unsigned
	{
		Void = 0,
		Mauer = 1,
		Wasser = 2,
		GrassHigh = 3,
		SnowTile = 4,
		FloorTilingStone = 5,
		FloorTilingStonePurple = 23,
		FloorTilingStoneOchre = 24,
		FloorTilingStoneSlate = 25,
		Coin = 6,
		Chest = 7,
		Rail = 8,
		MauerCracked = 9,
		FloorTilingRock = 10,
		FloorTilingRockCracked = 22,
		Lettuce = 11,
		Player = 12,
		Player2 = 13,
		Player3 = 14,
		Player4 = 15,
		ChestSilver = 16,
		ChestGold = 17,
		MauerLoose = 20,
		Door = 21,
		BarrelWood = 26,
		BarrelIndigo = 27,
		Dalek = 34,
		FlameGhost = 35,
		Schaedel = 4042

	};

	Dings(Dings::DingIDs dingID, Platform::String^ dingName, std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);

	Dings::DingIDs								ID();
	Platform::String^							Name();
	virtual bool								IsMob();
	void										Draw(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, int canvasX, int canvasY);
	D2D1_POINT_2U								GetSheetPlacement(Facings orientation);
	D2D1_POINT_2U								GetSheetPlacement(OrientabilityIndexRotatory rotationDent);
	D2D1_SIZE_U									GetExtentOnSheet();
	Layers										GetPreferredLayer();
	Facings										AvailableFacings();
	ObjectBehaviors								GetInherentBehaviors();
	bool										BoundingIsDefaultRect();
	D2D1_RECT_F									GetBoundingOuterRim();
	std::shared_ptr<Bounding>					GetBoundingInfo();
	bool										CouldCoalesce();
	static Facings								RotateFromFacing(Facings fromFacing, bool inverseDirection);
	static OrientabilityIndexRotatory			HeadingFromFacing(Facings orientationFacing);
	static Facings								FacingFromHeading(OrientabilityIndexRotatory orientabilityIndex);
	static Facings								RotateMobFine(Facings currentFacing, bool counterClockwise);

	static inline OrientabilityIndexRotatory	HeadingFromVector(float vectorX, float vectorY)
	{
		/* unity atan evaluates to 135° south east; since we use aviation headings,
		 * we flip atan2 the bird to land it into the right quadrant */
		auto scaledAngle = int((atan2(-vectorX, vectorY) + static_cast<float>(M_PI)) * Dings::sectorScale) % static_cast<int>(OrientabilityIndexRotatory::NumberOfSectors);
		return static_cast<OrientabilityIndexRotatory>(scaledAngle);
	};

protected:
	DingIDs				m_ID;
	Platform::String^	m_Name;
	std::shared_ptr<BrushRegistry> m_Brushes;
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
	D2D1_POINT_2U		m_lookupViech[OrientabilityIndexRotatory::NumberOfSectors];

	virtual Platform::String^ ShouldLoadFromBitmap();
	void PrepareRect(D2D1_POINT_2U *lookupLocation, D2D1_RECT_F &rectToSet);
	void PrepareRect7x7(D2D1_POINT_2U *lookupLocation, D2D1_RECT_F &rectToSet);
	void Rotate90(ID2D1RenderTarget *rendEr, D2D1_RECT_F rect, int rotateTimes);
	void Rotate(ID2D1RenderTarget *rendEr, D2D1_RECT_F rect, float rotationAngle);
	Microsoft::WRL::ComPtr<ID2D1Bitmap> LoadBitmap(Platform::String^ fileName);
	virtual void PrepareBackground(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) { };
	virtual void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo);
	virtual void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, const D2D1_RECT_F *rect);

protected:
	static const int Dings::MAUER_GRAY_SOLID;
	static const int Dings::MAUER_GRAY_LOOSE;
	static const float Dings::HAIRLINE;
	static const float Dings::WEDGE_STROKE;
	static const float Dings::MAUER_STROKE;
	static const float Dings::MAUER_INDENT;
	static const float Dings::MAUER_BEZIER_X;
	static const float Dings::MAUER_BEZIER_Y;

	std::shared_ptr<DX::DeviceResources>	m_deviceResources;
	D2D1_SIZE_U								m_extentOnSheet;
	ObjectBehaviors							m_Behaviors;
	std::shared_ptr<Bounding>				m_Bounding;

private:
	void SetOnSheetPlacementsFromCoalescability();
	Microsoft::WRL::ComPtr<ID2D1Bitmap> LoadFromBitmap();
	void Pack7x7(unsigned offsetX, unsigned offsetY, unsigned* x, unsigned* y);
	unsigned Pack7x7X(unsigned offsetX, unsigned offsetY, unsigned* x, unsigned* y);
	unsigned Pack7x7Y(unsigned offsetX, unsigned offsetY, unsigned* x, unsigned* y);
	void DrawShy(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo);
	void DrawTwin(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo);
	void DrawQuadruplet(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo);
	void DrawRotatory(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo);
	void DrawClumsyPack(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo);
	unsigned StepX(unsigned *oldValue);

	Platform::String^ m_fromFile;
};

class Mauer : public Dings 
{
public:
	Mauer(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
protected:
	void Mauer::PrepareBackground(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
};

class CrackedMauer : public Mauer
{
public:
	CrackedMauer(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	void PrepareBackground(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
};

class LooseMauer : public Dings
{
public:
	LooseMauer(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
protected:
	void LooseMauer::PrepareBackground(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
};

class Door : public Dings
{
public:
	Door(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, const D2D1_RECT_F *rect) override;
protected:
};

class Wasser : public Dings
{
public:
	Wasser(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
};

class HighGrass : public Dings
{
public:
	HighGrass(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Snow : public Dings
{
public:
	Snow(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class FloorStoneTile : public Dings
{
public:
	FloorStoneTile(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class FloorStoneTilePurple : public Dings
{
public:
	FloorStoneTilePurple(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class FloorStoneTileOchre : public Dings
{
public:
	FloorStoneTileOchre(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class FloorStoneTileSlate : public Dings
{
public:
	FloorStoneTileSlate(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class FloorRockTile : public Dings
{
public:
	FloorRockTile(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class FloorRockTileCracked : public Dings
{
public:
	FloorRockTileCracked(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Coin : public Dings
{
public:
	Coin(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
};

class Chest : public Dings
{
public:
	Chest(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, const D2D1_RECT_F *rect) override;

protected:
	virtual int ColorVariation();
};

class SilverChest : public Chest
{
public:
	SilverChest(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);

protected:
	int ColorVariation() override;
};


class GoldChest : public Chest
{
public:
	GoldChest(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);

protected:
	int ColorVariation() override;
};

class Lettuce : public Dings
{
public:
	Lettuce(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class BarrelWooden : public Dings
{
public:
	BarrelWooden(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class BarrelIndigo : public Dings
{
public:
	BarrelIndigo(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Rail : public Dings
{
public:
	Rail(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, const D2D1_RECT_F *rect) override;
};