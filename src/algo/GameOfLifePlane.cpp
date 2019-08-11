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

MFARGB GameOfLifePlane::GetColorSprinkler()
{
	return m_defaultColorSprinkler;
}
