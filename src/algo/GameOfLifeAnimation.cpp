#include "..\PreCompiledHeaders.h"
#include <DirectXColors.h> // For named colors
#include "..\app\blooDot.h"
#include "GameOfLife.h"
#include <memory>
#include <fstream>

using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;
using namespace std;
using namespace concurrency;

GameOfLifeAnimation::GameOfLifeAnimation()
{
	m_initialMatrix = m_currentMatrix = NULL;
	CancelRecording();
}

GameOfLifeAnimation::~GameOfLifeAnimation()
{
	CancelRecording();
	delete m_initialMatrix;
	delete m_currentMatrix;
}

void GameOfLifeAnimation::Reset(int matrixWidth, int matrixHeight)
{
	if (m_initialMatrix != NULL && m_initialMatrix->GetWidth() == matrixWidth && m_initialMatrix->GetHeight() == matrixHeight) 
	{
		m_initialMatrix->Wipe();
	}
	else
	{
		m_initialMatrix = new GameOfLifePlane(matrixWidth, matrixHeight);
	}

	if (m_currentMatrix != NULL && m_currentMatrix->GetWidth() == matrixWidth && m_currentMatrix->GetHeight() == matrixHeight)
	{
		m_currentMatrix->Wipe();
	}
	else 
	{
		m_currentMatrix = new GameOfLifePlane(matrixWidth, matrixHeight);
	}

	m_Steps.clear();
	m_playbackDirection = GameOfLifeAnimation::PlaybackDirection::Forward;
}

void GameOfLifeAnimation::CancelRecording()
{
	m_currentStepIndex = 0;
	m_startedRecordingAtStepIndex = -1;
	m_endedRecordingAtStepIndex = -1;
	delete m_recordingStartSnapshot;
}

void GameOfLifeAnimation::SetInitialMatrix(GameOfLifePlane* fromMatrix)
{
	m_initialMatrix = new GameOfLifePlane(fromMatrix->GetWidth(), fromMatrix->GetHeight());
	fromMatrix->CopyTo(m_initialMatrix);
}

void GameOfLifeAnimation::StartRecording()
{
	if (!m_IsRecording)
	{
		m_startedRecordingAtStepIndex = m_currentStepIndex;
		m_recordingStartSnapshot = new GameOfLifePlane(m_currentMatrix->GetWidth(), m_currentMatrix->GetHeight());
		m_currentMatrix->CopyTo(m_recordingStartSnapshot);
		m_IsRecording = true;
	}
}

void GameOfLifeAnimation::EndRecording()
{
	if (m_IsRecording)
	{
		m_IsRecording = false;
		m_endedRecordingAtStepIndex = m_currentStepIndex;
	}
}

void GameOfLifeAnimation::LoadRecording(Platform::String^ fileName)
{
	unsigned long long offset = 0L;
	GameOfLifeStep stepItem;
	GameOfLifeTransition transitionItem;

	Platform::Array<byte>^ rawData = m_basicReaderWriter->ReadData(fileName);
	byte* srcData = rawData->Data;

	/* 1. ask the file how big the matrix is */
	uint32 fileSignature = *reinterpret_cast<uint32*>(srcData + offset); offset += sizeof(uint32);
	uint32 boardWidth = *reinterpret_cast<uint32*>(srcData + offset); offset += sizeof(uint32);
	uint32 boardHeight = *reinterpret_cast<uint32*>(srcData + offset); offset += sizeof(uint32);

	/* 2. create an initial matrix of this size */
	Reset(boardWidth, boardHeight);

	/* 3. read the other properties */
	MFARGB colorCell = *reinterpret_cast<MFARGB*>(srcData + offset); offset += sizeof(MFARGB);
	MFARGB colorRain = *reinterpret_cast<MFARGB*>(srcData + offset); offset += sizeof(MFARGB);
	m_initialMatrix->SetDefaultColors(colorCell, colorRain);

	/* 4. read the steps. the first step is always the set of transitions
	 * to get from an empty board to the initial board. if the initial board is an empty board,
	 * it may still be a valid recording assuming manually placed items. but the fact that
	 * the initial board is empty, needs to be tracked in the savefile. */
	bool initialIsEmpty = *reinterpret_cast<bool*>(srcData + offset); offset += sizeof(bool);

	/* 5. finally, read the steps with each of their transitions */
	uint32 numSteps = *reinterpret_cast<uint32*>(srcData + offset); offset += sizeof(uint32);
	for (uint32 i = 0; i < numSteps; ++i)
	{
		int transitionsInStep;
		offset += GameOfLifeStep::FromBytes(srcData, offset, &stepItem, &transitionsInStep);
		for (int t = 0; t < transitionsInStep; ++t)
		{
			Transition transition = *reinterpret_cast<Transition*>(srcData + offset); offset += sizeof(Transition);
			offset += GameOfLifeTransition::FromBytes(srcData, offset, &transitionItem);
			stepItem.SetTransition(transition, transitionItem);
		}

		if (i == 0 && initialIsEmpty == false)
		{
			stepItem.ApplyStepTo(m_initialMatrix);
		}
		else 
		{
			m_Steps.push_back(stepItem);
		}
	}	

	/* clone this initial status to the current status */
	m_initialMatrix->CopyTo(m_currentMatrix);

}

GameOfLifePlane* GameOfLifeAnimation::GetCurrentMatrix()
{
	return m_currentMatrix;
}

void GameOfLifeAnimation::SinlgeStep()
{
	GameOfLifeStep stepItem;
	if (m_currentMatrix == NULL)
	{
		m_currentMatrix = new GameOfLifePlane(m_initialMatrix->GetWidth(), m_initialMatrix->GetHeight());
		m_initialMatrix->CopyTo(m_currentMatrix);
	}

	/* get a bunch of new transitions */
	++m_currentStepIndex;
	if (StepIsPlaybackFromRecording())
	{
		stepItem = m_Steps[m_currentStepIndex];
	}
	else
	{
		GameOfLifeStep step = ComputeFromCurrent();
		step.ApplyStepTo(m_currentMatrix);
		m_Steps.push_back(step);
	}

	stepItem.ApplyStepTo(m_currentMatrix);
}

void GameOfLifeAnimation::SaveRecording(Platform::String^ fileName)
{
	std::ofstream oF(fileName->Data());
	oF << blooDot::blooDotMain::BLOODOTFILE_SIGNATURE;

	oF.flush();
	oF.close();	
}

// private

bool GameOfLifeAnimation::StepIsPlaybackFromRecording()
{
	return m_currentStepIndex >= m_startedRecordingAtStepIndex && m_currentStepIndex <= m_endedRecordingAtStepIndex;
}

GameOfLifeStep GameOfLifeAnimation::ComputeFromCurrent()
{
	GameOfLifeStep step;

	/*
		Births: Each dead cell adjacent to exactly three live neighbors will become live in the next generation.
		Death by isolation: Each live cell with one or fewer live neighbors will die in the next generation.
		Death by overcrowding: Each live cell with four or more live neighbors will die in the next generation.
		Survival: Each live cell with either two or three live neighbors will remain alive for the next generation.
	*/

	for (int i = 0; i < m_currentMatrix->GetWidth(); ++i) for (int j = 0; j < m_currentMatrix->GetHeight(); ++j)
	{
		bool curState = m_currentMatrix->CellAt(i, j)->IsAlive();
		int NAl = m_currentMatrix->NeighborsAlive(i, j);
		if (curState)
		{
			if (NAl < 2)
			{
				/* starvation */
				step.AddTransition(Transition::DeceaseStarved, i, j, m_currentMatrix->GetColorCell());
			}
			else if (NAl > 3)
			{
				/* overcrowded */
				step.AddTransition(Transition::DeceaseOvercrowded, i, j, m_currentMatrix->GetColorCell());
			}
		}
		else
		{
			/* birth out of thin air */
			if (NAl == 3)
			{
				step.AddTransition(Transition::ComeToLife, i, j, m_currentMatrix->GetColorCell());
			}
		}
	}

	return step;
}

