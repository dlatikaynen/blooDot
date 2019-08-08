#include "..\PreCompiledHeaders.h"
#include <DirectXColors.h> // For named colors
#include "GameOfLifePlane.h"

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