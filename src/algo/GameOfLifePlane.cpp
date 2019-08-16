#include "..\PreCompiledHeaders.h"
#include <DirectXColors.h> // For named colors
#include "GameOfLife.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;

GameOfLifePlane::GameOfLifePlane() :
	m_Width(0),
	m_Height(0)
{
}

GameOfLifePlane::GameOfLifePlane(const int width, const int height) :
	m_Width(width),
	m_Height(height)
{
	m_Matrix = new GameOfLifeCell[width * height];
}

GameOfLifePlane::~GameOfLifePlane()
{
	delete[] m_Matrix;
}

int GameOfLifePlane::GetWidth()
{
	return m_Width;
}

int GameOfLifePlane::GetHeight()
{
	return m_Height;
}

bool GameOfLifePlane::IsEmpty()
{
	if (m_Width > 0 && m_Height > 0)
	{
		for (int i = 0; i < (m_Width * m_Height); ++i)
		{
			if ((&m_Matrix[i])->IsAlive())
			{
				return false;
			}
		}
	}

	return true;
}

GameOfLifeCell* GameOfLifePlane::CellAt(int x, int y)
{
	return &m_Matrix[y*GetWidth() + x];
}

void GameOfLifePlane::SetAlive(int x, int y)
{
	SetAlive(x, y, true);
}

void GameOfLifePlane::SetAlive(int x, int y, bool isAlive)
{
	GameOfLifeCell* cell = CellAt(x, y);
	if (isAlive) 
	{
		cell->SetAlive(true, m_defaultColorCell);
	}
	else
	{
		cell->SetAlive(false);
	}
}

void GameOfLifePlane::SetRaindrop(int x, int y, bool isRaindrop)
{
	GameOfLifeCell* cell = CellAt(x, y);
	if (isRaindrop)
	{
		cell->SetColor(m_defaultColorRain);
	}

	cell->SetRaindrop(isRaindrop);
}

void GameOfLifePlane::Wipe() 
{
	ForAll(GameOfLifeCell::Wipe);
}

void GameOfLifePlane::ForAll(void(*action)(GameOfLifeCell *))
{
	int width = GetWidth();
	int height = GetHeight();
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			action(CellAt(x, y));
		}
	}
}

void GameOfLifePlane::SetDefaultColors(MFARGB cellColor, MFARGB rainColor)
{
	m_defaultColorCell = cellColor;
	m_defaultColorRain = rainColor;
}

MFARGB GameOfLifePlane::GetColorCell()
{
	return m_defaultColorCell;
}

MFARGB GameOfLifePlane::GetColorRain()
{
	return m_defaultColorRain;
}

MFARGB GameOfLifePlane::GetColorSprinkler()
{
	return m_defaultColorSprinkler;
}

void GameOfLifePlane::CopyTo(GameOfLifePlane* destPlane)
{
	destPlane->SetDefaultColors(m_defaultColorCell, m_defaultColorRain);
	for (int x = 0; x < m_Width; ++x)
	{
		for (int y = 0; y < m_Height; ++y)
		{
			destPlane->SetAlive(x, y, CellAt(x, y)->IsAlive());
			destPlane->SetRaindrop(x, y, CellAt(x, y)->IsRaindrop());
		}
	}
}

bool GameOfLifePlane::NeighborN(int i, int j)
{
	return CellAt(i, IndexUp(j))->IsAlive();
}

bool GameOfLifePlane::NeighborS(int i, int j)
{
	return CellAt(i, IndexDown(j))->IsAlive();
}

bool GameOfLifePlane::NeighborE(int i, int j)
{
	return CellAt(IndexRight(i), j)->IsAlive();
}

bool GameOfLifePlane::NeighborNE(int i, int j)
{
	return CellAt(IndexRight(i), IndexUp(j))->IsAlive();
}

bool GameOfLifePlane::NeighborSE(int i, int j)
{
	return CellAt(IndexRight(i), IndexDown(j))->IsAlive();
}

bool GameOfLifePlane::NeighborW(int i, int j)
{
	return CellAt(IndexLeft(i), j)->IsAlive();
}

bool GameOfLifePlane::NeighborSW(int i, int j)
{
	return CellAt(IndexLeft(i), IndexDown(j))->IsAlive();
}

bool GameOfLifePlane::NeighborNW(int i, int j)
{
	return CellAt(IndexLeft(i), IndexUp(j))->IsAlive();
}

int GameOfLifePlane::NeighborsAlive(int i, int j)
{
	int na = 0;
	if (NeighborN(i, j)) ++na;
	if (NeighborE(i, j)) ++na;
	if (NeighborS(i, j)) ++na;
	if (NeighborW(i, j)) ++na;
	if (na == 4) 
	{
		/* optimization: overcrowding inevitable */
		return na;
	}

	if (NeighborNE(i, j)) ++na;
	if (na == 4)
	{
		/* optimization: overcrowding inevitable */
		return na;
	}

	if (NeighborNW(i, j)) ++na;
	if (na == 4)
	{
		/* optimization: overcrowding inevitable */
		return na;
	}

	if (NeighborSE(i, j)) ++na;
	if (na == 0 || na == 4)
	{
		/* optimization: starvation or overcrowding inevitable */
		return na;
	}

	if (NeighborSW(i, j)) ++na;

	return na;
}

int GameOfLifePlane::IndexLeft(int i)
{
	if (i == 0)
	{
		return m_Width - 1;
	}

	return i - 1;
}

int GameOfLifePlane::IndexRight(int i)
{
	if (i == m_Width - 1)
	{
		return 0;
	}

	return i + 1;
}

int GameOfLifePlane::IndexUp(int j)
{
	if (j == 0)
	{
		return m_Height - 1;
	}

	return j - 1;
}

int GameOfLifePlane::IndexDown(int j)
{
	if (j == m_Height - 1)
	{
		return 0;
	} 

	return j + 1;
}
