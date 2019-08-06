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
	void MakeRaindrop();
	bool IsRaindrop();
	bool IsPromotedRaindrop();

private:
	bool m_isAlive;
	bool m_isRaindrop;
	bool m_isPromotedRaindrop;
	int m_birthCountdown;
	int m_demiseCountdown;
	float m_slightlyOffX;
	float m_slightlyOffY;
};
