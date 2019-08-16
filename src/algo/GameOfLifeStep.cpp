#include "..\PreCompiledHeaders.h"
#include <DirectXColors.h> // For named colors
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

GameOfLifeStep::GameOfLifeStep()
{

}

int GameOfLifeStep::FromBytes(byte* srcData, unsigned long long offset, _Out_ GameOfLifeStep* decodedStep, _Out_ int* numberOfTransitionKeysInStep)
{
	int offsetDelta = 0;;
	GameOfLifeStep step;
	byte savedTranKeyCount = *reinterpret_cast<byte*>(srcData + offset + offsetDelta); offsetDelta += sizeof(byte);
	*numberOfTransitionKeysInStep = safe_cast<int>(savedTranKeyCount);
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

GameOfLifeStep GameOfLifeStep::ExtractFromPlaneStatus(GameOfLifePlane* statusPlane)
{
	GameOfLifeStep step;
	Transition transition;

	for (int x = 0; x < statusPlane->GetWidth(); ++x)
	{
		for (int y = 0; y < statusPlane->GetHeight(); ++y)
		{
			auto cell = statusPlane->CellAt(x, y);
			if (cell->IsAlive())
			{
				if (cell->IsRaindrop())
				{
					transition = Transition::RainedDown;
				}
				else
				{
					transition = Transition::ManuallySpawned;
				}

				step.AddTransition(transition, x, y, cell->GetCurrentColor());
			}
		}
	}

	return step;
}

void GameOfLifeStep::StepToFile(ofstream* toFile)
{
	/* this basically consists only of the count of atoms per transition */
	byte tranKeyCount = static_cast<byte>(m_Transitions.size());
	toFile->write((char *)&tranKeyCount, sizeof(byte));
	
	/* and of course, the atoms grouped by transitions themselves */
	for (auto iter = m_Transitions.begin(); iter != m_Transitions.end(); ++iter)
	{
		Transition transitionKey = iter->first;
		toFile->write((char*)&transitionKey, sizeof(transitionKey));

		GameOfLifeTransition transitionItem = iter->second;
		transitionItem.TransitionToFile(toFile);
	}
}