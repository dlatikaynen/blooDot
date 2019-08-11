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

	m_Transitions.clear();
	m_playbackDirection = GameOfLifeAnimation::PlaybackDirection::Forward;
}

void GameOfLifeAnimation::LoadFromFile(Platform::String^ fileName)
{
	unsigned long long offset = 0L;
	GameOfLifeTransition transition;

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

	/* 4. finally, read the transitions */
	uint32 numTransitions = *reinterpret_cast<uint32*>(srcData + offset); offset += sizeof(uint32);
	for (int i = 0; i < numTransitions; ++i)
	{
		offset += transition.FromBytes(srcData + offset, &transition);
		m_Transitions.push_back(transition);
	}

	/* clone this initial status to the current status */

}

GameOfLifePlane* GameOfLifeAnimation::GetCurrentMatrix()
{
	return m_currentMatrix;
}

void GameOfLifeAnimation::SinlgeStep()
{
	/* get a bunch of new transitions */
}

void GameOfLifeAnimation::SaveToFile(Platform::String^ fileName)
{

}
