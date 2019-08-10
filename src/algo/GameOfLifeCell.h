#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"

class GameOfLifeCell
{
public:
	GameOfLifeCell::GameOfLifeCell();
	GameOfLifeCell::GameOfLifeCell(const bool rainedDown);
	bool IsAlive();
	void SetAlive(bool isAlive);
	void SetRaindrop(bool isRaindrop);
	void MakeRaindrop(bool isRaindrop);
	bool IsRaindrop();
	bool IsPromotedRaindrop();
	void DrawCell(Microsoft::WRL::ComPtr<ID2D1DeviceContext> dxDC, int x, int y, int cellSideLength, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushCell, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushRaindrop);

private:
	bool m_isAlive;
	bool m_isRaindrop;
	bool m_isPromotedRaindrop;
	int m_birthCountdown;
	int m_demiseCountdown;
	float m_slightlyOffX;
	float m_slightlyOffY;
};
