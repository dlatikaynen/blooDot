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

	static int FromBytes(byte* inBytes, unsigned long long offset, _Out_ GameOfLifeTransition* transition);
	void AddAtom(int x, int y, MFARGB color);
	GameOfLifeTransitionAtoms GetAtoms();
	byte* ToBytes();

private:
	GameOfLifeTransitionAtoms m_Coordinates;
};

typedef std::unordered_map<Transition, GameOfLifeTransition> GameOfLifeTransitions;

class GameOfLifeStep
{
public:
	GameOfLifeStep::GameOfLifeStep();
	static int FromBytes(byte* srcData, unsigned long long offset, _Out_ GameOfLifeStep* decodedStep, _Out_ int* numberOfTransitionsInStep);
	void AddTransition(Transition transition, int x, int y, MFARGB color);
	void SetTransition(Transition transition, GameOfLifeTransition transitionToAdd);
	GameOfLifeTransitions GetTransitions();
	void ApplyStepTo(GameOfLifePlane* targetPlane);
	void ToFile(Microsoft::WRL::Wrappers::FileHandle* fileHandle);

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
	void CancelRecording();
	void SinlgeStep();
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
	GameOfLifeTransitions ComputeFromCurrent();
};
