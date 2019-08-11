#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\dx\BrushRegistry.h"

class GameOfLifeCell
{
public:
	GameOfLifeCell::GameOfLifeCell();

	bool IsAlive();
	void SetAlive(bool isAlive);
	void SetAlive(bool isAlive, MFARGB color);
	void SetColor(MFARGB color);
	void SetRaindrop(bool isRaindrop);
	void MakeRaindrop(bool isRaindrop);
	bool IsRaindrop();
	bool IsPromotedRaindrop();
	void DrawCell(Microsoft::WRL::ComPtr<ID2D1DeviceContext> dxDC, int x, int y, int cellSideLength, BrushRegistry* brushRegistry);
	static void Wipe(GameOfLifeCell* cell);

private:
	MFARGB m_Color;
	bool m_isAlive;
	bool m_isRaindrop;
	bool m_isPromotedRaindrop;
	int m_birthCountdown;
	int m_demiseCountdown;
	float m_slightlyOffX;
	float m_slightlyOffY;
};

class GameOfLifePlane
{
public:
	GameOfLifePlane::GameOfLifePlane();
	GameOfLifePlane::GameOfLifePlane(const int width, const int height);
	GameOfLifePlane::~GameOfLifePlane();

	int GetWidth();
	int GetHeight();
	GameOfLifeCell* CellAt(int x, int y);
	void SetAlive(int x, int y);
	void SetAlive(int x, int y, bool isAlive);
	void SetRaindrop(int x, int y, bool isRaindrop);
	void Wipe();
	void ForAll(void(*action)(GameOfLifeCell*));
	MFARGB GetColorSprinkler();

private:
	int m_Width;
	int m_Height;
	GameOfLifeCell* m_Matrix;
	MFARGB m_defaultColorCell = { 127,250,19,255 };
	MFARGB m_defaultColorRain = { 200,0,0,255 };
	MFARGB m_defaultColorSprinkler = { 192, 192, 192, 96 };
};
