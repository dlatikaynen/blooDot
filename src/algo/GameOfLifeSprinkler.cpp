#include "..\PreCompiledHeaders.h"
#include "..\dx\BrushRegistry.h"
#include <DirectXColors.h> // For named colors
#include "GameOfLifeSprinkler.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;

GameOfLifeSprinkler::GameOfLifeSprinkler()
{
	m_Square = NULL;
}

GameOfLifeSprinkler::~GameOfLifeSprinkler()
{
	delete m_Square;
}

void GameOfLifeSprinkler::Create(int radius)
{
	/* the radius is adjusted so we always have a middle point,
	* that is, an odd number of diameter pixels, example 4
	*
	*			###
	*          #####
	*         #######
	*        ####X####
	*         #######
	*          #####
	*           ###
	*/

	m_Radius = radius;
	m_sideLength = m_Radius * 2;
	if (m_sideLength % 2 == 0)
	{
		m_sideLength++;
	}

	D2D1_POINT_2L midPoint = D2D1::Point2<int>(m_sideLength / 2 + 1, m_sideLength / 2 + 1);
	D2D1_POINT_2L point;
	m_radiusSquared = m_Radius * m_Radius;
	m_Square = new GameOfLifePlane(m_sideLength, m_sideLength);	
	for (int x = 0; x < m_sideLength; ++x)
	{
		for (int y = 0; y < m_sideLength; ++y)
		{
			GameOfLifeCell* cell = m_Square->CellAt(x, y);
			cell->SetColor(m_Square->GetColorSprinkler());
			point.x = x - m_Radius;
			point.y = y - m_Radius;
			if (PointInCircle(point) == false)
			{
				m_Square->CellAt(x, y)->SetAlive(false);
			}
		}
	}
}

void GameOfLifeSprinkler::Render(Microsoft::WRL::ComPtr<ID2D1DeviceContext> dxDC, int midPointX, int midPointY, int cellSideLength, BrushRegistry* brushRegistry)
{
	int cellStride = cellSideLength + 1;

	for (int x = 0; x < m_sideLength; ++x)
	{
		for (int y = 0; y < m_sideLength; ++y)
		{
			GameOfLifeCell* cell = m_Square->CellAt(x, y);
			if (cell->IsAlive())
			{
				cell->DrawCell(
					dxDC,
					(midPointX / cellStride) + x - m_Radius,
					(midPointY / cellStride) + y - m_Radius,
					cellSideLength,
					brushRegistry
				);
			}
		}
	}
}

bool GameOfLifeSprinkler::PointInCircle(D2D1_POINT_2L point)
{
	/* the membership function for a circle is pythagorean,
	 * using inner orth. triangles so we have x² + y² = r² 
	 * radius is hypothenuse */
	int aSq = point.x * point.x;
	int bSq = point.y * point.y;
	
	return (aSq + bSq) <= m_radiusSquared;
}
