#include "..\PreCompiledHeaders.h"
#include <DirectXColors.h> // For named colors
#include "..\app\blooDot.h"
#include "GameOfLife.h"

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
	m_currentStepIndex = -1;
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
	CancelRecording();
	m_playbackDirection = GameOfLifeAnimation::PlaybackDirection::Forward;
}

void GameOfLifeAnimation::CancelRecording()
{
	m_currentStepIndex = -1;
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
		m_recordingStartSnapshot = new GameOfLifePlane(GetCurrentMatrix()->GetWidth(), GetCurrentMatrix()->GetHeight());
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

	uint64 fileSignature = *reinterpret_cast<uint64*>(srcData + offset); offset += sizeof(uint64);
	uint64 signatToMatch = (uint64_t)(*(uint64_t*)&blooDot::blooDotMain::BLOODOTFILE_SIGNATURE[0]);
	if (fileSignature != signatToMatch)
	{
		return;
	}

	unsigned char sigByte = *reinterpret_cast<unsigned char*>(srcData + offset); offset += sizeof(unsigned char);
	if (sigByte != GameOfLifeAnimation::sigbyte)
	{
		return;
	}

	uint16 fileType = *reinterpret_cast<uint16*>(srcData + offset); offset += sizeof(uint16);
	uint16 tToMatch = (uint16_t)(*(uint16_t*)&blooDot::blooDotMain::BLOODOTFILE_CONTENTTYPE_GOLANIMATION[0]);
	if (fileType != tToMatch)
	{
		return;
	}

	/* ask the file how big the matrix is */
	unsigned short boardWidth = *reinterpret_cast<unsigned short*>(srcData + offset); offset += sizeof(unsigned short);
	unsigned short boardHeight = *reinterpret_cast<unsigned short*>(srcData + offset); offset += sizeof(unsigned short);

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
			Transition transitionKey;
			offset += GameOfLifeTransition::FromBytes(srcData, offset, &transitionKey, &transitionItem);
			stepItem.SetTransition(transitionKey, transitionItem);
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
	if (m_currentMatrix == NULL)
	{
		m_currentMatrix = new GameOfLifePlane(m_initialMatrix->GetWidth(), m_initialMatrix->GetHeight());
		m_initialMatrix->CopyTo(m_currentMatrix);
	}

	return m_currentMatrix;
}

void GameOfLifeAnimation::SingleStep()
{
	GameOfLifeStep stepItem;

	/* get a bunch of new transitions */
	if (m_currentStepIndex < 0)
	{
		m_currentStepIndex = 0;
		if (m_IsRecording && m_startedRecordingAtStepIndex < 0) 
		{
			m_startedRecordingAtStepIndex = m_currentStepIndex;
		}
	}
	else
	{
		++m_currentStepIndex;
	}

	if (StepIsPlaybackFromRecording())
	{
		stepItem = m_Steps[m_currentStepIndex];
	}
	else
	{
		stepItem = ComputeFromCurrent();
		m_Steps.push_back(stepItem);
	}

	stepItem.ApplyStepTo(m_currentMatrix);
}

bool GameOfLifeAnimation::IsRecording()
{
	return m_IsRecording;
}

unsigned int GameOfLifeAnimation::GetNumStepsRecorded()
{
	if (m_startedRecordingAtStepIndex < 0)
	{
		return 0;
	}
	else if (m_IsRecording) 
	{
		return m_currentStepIndex - m_startedRecordingAtStepIndex + 1;
	}

	return m_endedRecordingAtStepIndex - m_startedRecordingAtStepIndex + 1;
}

void GameOfLifeAnimation::SaveRecording(Platform::String^ fileName)
{
	if (m_IsRecording || m_startedRecordingAtStepIndex < 0)
	{
		return;
	}

	std::wstring fPath = ApplicationData::Current->RoamingFolder->Path->Data();
	std::wstring sPath = fPath.append(L"\\");
	std::wstring fName = sPath.append(fileName->Data());
	std::ofstream oF;
	oF.open(fName, ios_base::out | ios_base::binary);
	oF.write((char*)&blooDot::blooDotMain::BLOODOTFILE_SIGNATURE, sizeof(blooDot::blooDotMain::BLOODOTFILE_SIGNATURE));
	const unsigned char sigbyte = GameOfLifeAnimation::sigbyte;
	oF.write((char *)&sigbyte, sizeof(unsigned char));
	oF.write((char*)&blooDot::blooDotMain::BLOODOTFILE_CONTENTTYPE_GOLANIMATION, sizeof(blooDot::blooDotMain::BLOODOTFILE_CONTENTTYPE_GOLANIMATION));
	const unsigned short uwidth = GetCurrentMatrix()->GetWidth();
	oF.write((char *)&uwidth, sizeof(unsigned short));
	const unsigned short uheight = GetCurrentMatrix()->GetHeight();
	oF.write((char *)&uheight, sizeof(unsigned short));
	const MFARGB ac1 = GetCurrentMatrix()->GetColorCell();
	oF.write((char *)&ac1, sizeof(MFARGB));
	const MFARGB ac2 = GetCurrentMatrix()->GetColorRain();
	oF.write((char *)&ac2, sizeof(MFARGB));
	const bool initialisempty = m_recordingStartSnapshot == NULL || m_recordingStartSnapshot->IsEmpty();
	oF.write((char *)&initialisempty, sizeof(bool));
	/* when we have a non-empty initial matrix, then we convert that to steps now,
	 * adding one to the overall step count */
	const uint32 stepCount = GetNumStepsRecorded() + initialisempty ? 1 : 0;
	oF.write((char *)&stepCount, sizeof(uint32));
	if (!initialisempty)
	{
		GameOfLifeStep step;
		step.ExtractFromPlaneStatus(m_recordingStartSnapshot).StepToFile(&oF);
	}

	/* write all the steps included in the recording */
	for (int i = m_startedRecordingAtStepIndex; i<= m_endedRecordingAtStepIndex; ++i)
	{
		m_Steps.at(i).StepToFile(&oF);
	}

	/* termination */
	oF.write((char *)&sigbyte, sizeof(unsigned char));
	oF.close();	
	if (!oF) 
	{
		OutputDebugStringW(L"mist.");
	}
}

// privates

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

	auto currentMatrix = GetCurrentMatrix();
	int width = currentMatrix->GetWidth();
	int height = currentMatrix->GetHeight();

	for (int i = 0; i < width; ++i) for (int j = 0; j < height; ++j)
	{
		bool curState = currentMatrix->CellAt(i, j)->IsAlive();
		int NAl = currentMatrix->NeighborsAlive(i, j);
		if (curState)
		{
			if (NAl < 2)
			{
				/* starvation */
				step.AddTransition(Transition::DeceaseStarved, i, j, currentMatrix->GetColorCell());
			}
			else if (NAl > 3)
			{
				/* overcrowded */
				step.AddTransition(Transition::DeceaseOvercrowded, i, j, currentMatrix->GetColorCell());
			}
		}
		else
		{
			/* birth out of thin air */
			if (NAl == 3)
			{
				step.AddTransition(Transition::ComeToLife, i, j, currentMatrix->GetColorCell());
			}
		}
	}

	return step;
}

