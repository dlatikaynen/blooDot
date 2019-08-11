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

GameOfLifeTransition::GameOfLifeTransition()
{

}

int GameOfLifeTransition::FromBytes(byte* inBytes, unsigned long long offset, _Out_ GameOfLifeTransition* transition)
{
	int offsetDelta = 0;
	GameOfLifeTransition tranItem;
	/* read the number of coordinates */
	uint32 savedAtomCount = *reinterpret_cast<uint32*>(inBytes + offset + offsetDelta); offsetDelta += sizeof(uint32);
	/* read each coordinate */
	for (uint32 i = 0; i < savedAtomCount; ++i)
	{
		uint32 x = *reinterpret_cast<uint32*>(inBytes + offset + offsetDelta); offsetDelta += sizeof(uint32);
		uint32 y = *reinterpret_cast<uint32*>(inBytes + offset + offsetDelta); offsetDelta += sizeof(uint32);
		MFARGB c = *reinterpret_cast<MFARGB*>(inBytes + offset + offsetDelta); offsetDelta += sizeof(MFARGB);
		tranItem.AddAtom(x, y, c);
	}

	transition = &tranItem;
	return offsetDelta;

}

void GameOfLifeTransition::AddAtom(int x, int y, MFARGB color)
{
	TransitionAtom atom;
	atom.Point.x = x;
	atom.Point.y = y;
	atom.Color = color;
	m_Coordinates.push_back(atom);
}

GameOfLifeTransitionAtoms GameOfLifeTransition::GetAtoms()
{
	return m_Coordinates;
}

byte* GameOfLifeTransition::ToBytes()
{
	return NULL;
}
