#pragma once

#include "..\io\BasicReaderWriter.h"
#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\dx\BrushRegistry.h"

#include <type_traits>
#include <iostream>
#include <fstream>

class GameOfLifeCell
{
public:
	GameOfLifeCell::GameOfLifeCell();

	bool IsAlive();
	void SetAlive(bool isAlive);
	void SetAlive(bool isAlive, MFARGB color);
	void SetColor(MFARGB color);
	MFARGB GetCurrentColor();
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
	bool IsEmpty();
	GameOfLifeCell* CellAt(int x, int y);
	int NeighborsAlive(int i, int j);
	void SetAlive(int x, int y);
	void SetAlive(int x, int y, bool isAlive);
	void SetRaindrop(int x, int y, bool isRaindrop);
	void Wipe();
	void ForAll(void(*action)(GameOfLifeCell*));
	MFARGB GetColorCell();
	MFARGB GetColorRain();
	MFARGB GetColorSprinkler();
	void SetDefaultColors(MFARGB cellColor, MFARGB rainColor);
	void CopyTo(GameOfLifePlane* destPlane);

private:
	int m_Width;
	int m_Height;
	GameOfLifeCell* m_Matrix;
	MFARGB m_defaultColorCell = { 127,250,19,255 };
	MFARGB m_defaultColorRain = { 200,0,0,255 };
	MFARGB m_defaultColorSprinkler = { 192, 192, 192, 96 };

	int IndexLeft(int i);
	int IndexRight(int i);
	int IndexUp(int j);
	int IndexDown(int j);

	bool NeighborN(int i, int j);
	bool NeighborS(int i, int j);
	bool NeighborE(int i, int j);
	bool NeighborNE(int i, int j);
	bool NeighborSE(int i, int j);
	bool NeighborW(int i, int j);
	bool NeighborNW(int i, int j);
	bool NeighborSW(int i, int j);
};

enum Transition
{
	ComeToLife = 1,
	DeceaseStarved = 2,
	DeceaseOvercrowded = 3,
	RainedDown = 4,
	ManuallySpawned = 5,
	ManuallyRemoved = 6
};

enum TransitionEffective
{
	GoNotAlive = 0,
	GoAlive = 1
};

typedef struct
{
	POINT Point;
	MFARGB Color;
} TransitionAtom;

typedef std::vector<TransitionAtom> GameOfLifeTransitionAtoms;

class GameOfLifeTransition
{
public:
	GameOfLifeTransition::GameOfLifeTransition();
	GameOfLifeTransition::~GameOfLifeTransition();

	static int FromBytes(byte* inBytes, unsigned long long offset, _Out_ Transition* transitionKey, _Out_ GameOfLifeTransition* transitionItem);
	void AddAtom(int x, int y, MFARGB color);
	GameOfLifeTransitionAtoms GetAtoms();
	void TransitionToFile(std::ofstream* toFile);

private:
	GameOfLifeTransitionAtoms m_Coordinates;
};

typedef std::unordered_map<Transition, GameOfLifeTransition> GameOfLifeTransitions;

class GameOfLifeStep
{
public:
	GameOfLifeStep::GameOfLifeStep();
	GameOfLifeStep::~GameOfLifeStep();

	static int FromBytes(byte* srcData, unsigned long long offset, _Out_ GameOfLifeStep* decodedStep, _Out_ int* numberOfTransitionKeysInStep);
	void AddTransition(Transition transition, int x, int y, MFARGB color);
	void SetTransition(Transition transition, GameOfLifeTransition transitionToAdd);
	GameOfLifeTransitions GetTransitions();
	void ApplyStepTo(GameOfLifePlane* targetPlane);
	GameOfLifeStep ExtractFromPlaneStatus(GameOfLifePlane* statusPlane);
	void StepToFile(std::ofstream* toFile);

private:
	GameOfLifeTransitions m_Transitions;
};

typedef std::vector<GameOfLifeStep> GameOfLifeSteps;

class GameOfLifeAnimation
{
public:
	GameOfLifeAnimation::GameOfLifeAnimation();
	GameOfLifeAnimation::~GameOfLifeAnimation();
	void Reset(int matrixWidth, int matrixHeight);
	void LoadRecording(Platform::String^ fileName);
	void SetInitialMatrix(GameOfLifePlane* fromMatrix);
	GameOfLifePlane* GetCurrentMatrix();
	void StartRecording();
	bool IsRecording();
	unsigned int GetNumStepsRecorded();
	void CancelRecording();
	void SingleStep();
	void EndRecording();
	void SaveRecording(Platform::String^ fileName);

	enum PlaybackDirection
	{
		Forward = 1,

		/* this is actually a hard problem: https://www.kaggle.com/c/conway-s-reverse-game-of-life
		 * for us, it is easy, because Dean Moriarty. */
		Backward = 2
	};

private:
	const unsigned char sigbyte = 42;

	BasicReaderWriter^ m_basicReaderWriter;
	GameOfLifePlane* m_initialMatrix;
	GameOfLifePlane* m_currentMatrix;
	GameOfLifePlane* m_recordingStartSnapshot;
	GameOfLifeSteps m_Steps;
	PlaybackDirection m_playbackDirection;
	bool m_IsRecording;
	int m_currentStepIndex;
	int m_startedRecordingAtStepIndex;
	int m_endedRecordingAtStepIndex;
	bool StepIsPlaybackFromRecording();
	GameOfLifeStep ComputeFromCurrent();
};
