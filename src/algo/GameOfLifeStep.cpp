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

GameOfLifeStep::GameOfLifeStep()
{

}

int GameOfLifeStep::FromBytes(byte* srcData, unsigned long long offset, _Out_ GameOfLifeStep* decodedStep, _Out_ int* numberOfTransitionsInStep)
{
	int offsetDelta = 0;;
	GameOfLifeStep step;
	uint32 savedTranCount = *reinterpret_cast<uint32*>(srcData + offset + offsetDelta); offsetDelta += sizeof(uint32);
	*numberOfTransitionsInStep = safe_cast<int>(savedTranCount);
	decodedStep = &step;
	
	return offsetDelta;
}

void GameOfLifeStep::AddTransition(Transition transition, int x, int y, MFARGB color)
{
	m_Transitions[transition].AddAtom(x, y, color);
}

void GameOfLifeStep::SetTransition(Transition transition, GameOfLifeTransition transitionItem)
{
	m_Transitions[transition] = transitionItem;
}

GameOfLifeTransitions GameOfLifeStep::GetTransitions()
{
	return m_Transitions;
}

void GameOfLifeStep::ApplyStepTo(GameOfLifePlane* targetPlane)
{
	for (auto kiter = m_Transitions.begin(); kiter != m_Transitions.end(); ++kiter)
	{
		Transition transition = kiter->first;
		GameOfLifeTransition transitionItem = kiter->second;
		GameOfLifeTransitionAtoms transitionAtoms = transitionItem.GetAtoms();
		for (auto titer = transitionAtoms.begin(); titer != transitionAtoms.end(); ++titer)
		{			
			GameOfLifeCell* cell = targetPlane->CellAt(titer->Point.x, titer->Point.y);			
			cell->SetAlive(
				transition == Transition::ComeToLife || transition == Transition::ManuallySpawned,
				titer->Color
			);
		}
	}
}

void GameOfLifeStep::ToFile(Microsoft::WRL::Wrappers::FileHandle* fileHandle)
{
	std::vector<byte> bytes;
	auto bRW = ref new BasicReaderWriter();
	/* this basically consists only of the measured transition count */
	uint32 tranCount = static_cast<uint32>(m_Transitions.size());
	bRW->WriteData(fileHandle, tranCount);
}