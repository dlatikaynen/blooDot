#include "..\PreCompiledHeaders.h"
#include <DirectXColors.h> // For named colors
#include "GameOfLifeCell.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;

const int GAMEOFLIFECELL_TRANSITION_FRAMES = 5;

GameOfLifeCell::GameOfLifeCell() :
	m_isRaindrop(false),
	m_isPromotedRaindrop(false),
	m_birthCountdown(0),
	m_demiseCountdown(0),
	m_isAlive(true)
{
	m_slightlyOffX = 0;
	m_slightlyOffY = 0;
}

GameOfLifeCell::GameOfLifeCell(const bool rainedDown) :
	m_isRaindrop(rainedDown),
	m_isPromotedRaindrop(false),
	m_birthCountdown(0),
	m_demiseCountdown(0),
	m_isAlive(true)
{
	m_slightlyOffX = 0;
	m_slightlyOffY = 0;
	if (rainedDown) 
	{
		MakeRaindrop(true);
	}
}

bool GameOfLifeCell::IsAlive()
{
	return m_isAlive;
}

bool GameOfLifeCell::IsRaindrop()
{
	return m_isRaindrop;
}

bool GameOfLifeCell::IsPromotedRaindrop()
{
	return m_isPromotedRaindrop;
}

void GameOfLifeCell::SetAlive(bool isAlive)
{
	m_demiseCountdown = isAlive ? 0 : GAMEOFLIFECELL_TRANSITION_FRAMES;
	m_birthCountdown = isAlive ? GAMEOFLIFECELL_TRANSITION_FRAMES : 0;
	m_isAlive = isAlive;
}

void GameOfLifeCell::SetRaindrop(bool isRaindrop)
{
	m_isRaindrop = isRaindrop;
}

void GameOfLifeCell::MakeRaindrop(bool isRaindrop)
{
	if (isRaindrop)
	{
		m_isAlive = true;
	}

	m_isRaindrop = isRaindrop;
}

void GameOfLifeCell::DrawCell(Microsoft::WRL::ComPtr<ID2D1DeviceContext> dxDC, int x, int y, int cellSideLength, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushCell, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushRaindrop)
{
	D2D1_RECT_F rect;
	int cellStride = cellSideLength + 1;
	rect.left = static_cast<float>(cellStride * x);
	rect.top = static_cast<float>(cellStride * y);
	rect.right = rect.left + cellSideLength;
	rect.bottom = rect.top + cellSideLength;
	D2D1_ROUNDED_RECT rrect;
	rrect.rect = rect;
	rrect.radiusX = 2;
	rrect.radiusY = 2;

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = this->IsRaindrop() ? brushRaindrop : brushCell;

	dxDC->FillRoundedRectangle(
		&rrect,
		brush.Get()
	);
}