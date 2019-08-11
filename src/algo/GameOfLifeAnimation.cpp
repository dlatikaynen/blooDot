#include "..\PreCompiledHeaders.h"
#include <DirectXColors.h> // For named colors
#include "GameOfLife.h"
#include <memory>

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

	/* get a bunch of new transitions */
	++m_currentStepIndex;
	if (StepIsPlaybackFromRecording())
	{
		stepItem = m_Steps[m_currentStepIndex];
	}
	else
	{
		//GameOfLifeTransitions transItems = ComputeFromCurrent();
		
	}

	stepItem.ApplyStepTo(m_currentMatrix);
}

void GameOfLifeAnimation::SaveRecording(Platform::String^ fileName)
{

}

// private

bool GameOfLifeAnimation::StepIsPlaybackFromRecording()
{
	return m_currentStepIndex >= m_startedRecordingAtStepIndex && m_currentStepIndex <= m_endedRecordingAtStepIndex;
}

GameOfLifeTransitions GameOfLifeAnimation::ComputeFromCurrent()
{
	return GameOfLifeTransitions();
}

//GameOfLifeTransitions GameOfLifeAnimation::ComputeFromCurrent()
//{
//	GameOfLifeTransitions transitions;
//
//	transitions[Transition::ComeToLife].AddAtom(1, 1, m_currentMatrix->GetColorCell());
//
//	return transitions;
//}

