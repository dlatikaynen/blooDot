#pragma once

#include "..\io\BasicReaderWriter.h"
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
	void SetDefaultColors(MFARGB cellColor, MFARGB rainColor);

private:
	int m_Width;
	int m_Height;
	GameOfLifeCell* m_Matrix;
	MFARGB m_defaultColorCell = { 127,250,19,255 };
	MFARGB m_defaultColorRain = { 200,0,0,255 };
	MFARGB m_defaultColorSprinkler = { 192, 192, 192, 96 };
};

class GameOfLifeTransition 
{
public:
	GameOfLifeTransition::GameOfLifeTransition();

	typedef std::vector<GameOfLifeTransition> GameOfLifeTransitions;

	uint32 FromBytes(byte* inBytes, _Out_ GameOfLifeTransition* transition);
	byte* ToBytes();

	enum Transition
	{
		ComeToLife = 1,
		DeceaseStarved = 2,
		DeceaseOvercrowded = 3,
		ManuallySpawned = 4,
		ManuallyRemoved = 5
	};

	enum TransitionEffective
	{
		GoNotAlive = 0,
		GoAlive = 1
	};

private:
	Transition m_Transition;
	bool m_cameAliveAsRaindrop;
	MFARGB m_cameAliveInColor;
	int m_planeX;
	int m_planeY;
};

class GameOfLifeAnimation
{
public:
	GameOfLifeAnimation::GameOfLifeAnimation();
	void Reset(int matrixWidth, int matrixHeight);
	void LoadFromFile(Platform::String^ fileName);
	GameOfLifePlane* GetCurrentMatrix();
	void SinlgeStep();
	void SaveToFile(Platform::String^ fileName);

	enum PlaybackDirection
	{
		Forward = 1,

		/* this is actually a hard problem: https://www.kaggle.com/c/conway-s-reverse-game-of-life
		 * for us, it is easy, because Dean Moriarty. */
		Backward = 2
	};

private:
	BasicReaderWriter^ m_basicReaderWriter;
	GameOfLifePlane* m_initialMatrix;
	GameOfLifePlane* m_currentMatrix;
	GameOfLifeTransition::GameOfLifeTransitions m_Transitions;
	PlaybackDirection m_playbackDirection;
	int currentTransition;
};