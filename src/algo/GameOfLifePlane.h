#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "GameOfLifeCell.h"

class GameOfLifePlane
{
public:
	GameOfLifePlane::GameOfLifePlane();
	GameOfLifePlane::GameOfLifePlane(const int width, const int height);
	GameOfLifePlane::~GameOfLifePlane();

	int GetWidth();
	int GetHeight();
	GameOfLifeCell* CellAt(int x, int y);

private:
	int m_Width;
	int m_Height;
	GameOfLifeCell* m_Matrix;
};
