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

int GameOfLifeTransition::FromBytes(byte* inBytes, unsigned long long offset, _Out_ Transition* transitionKey, _Out_ GameOfLifeTransition* transitionItem)
{
	int offsetDelta = 0;
	GameOfLifeTransition tranItem;
	/* read the number of coordinates */
	Transition tranKey = *reinterpret_cast<Transition*>(inBytes + offset + offsetDelta); offsetDelta += sizeof(Transition);
	unsigned short savedAtomCount = *reinterpret_cast<unsigned short*>(inBytes + offset + offsetDelta); offsetDelta += sizeof(unsigned short);
	/* read each coordinate */
	for (uint32 i = 0; i < savedAtomCount; ++i)
	{
		unsigned short x = *reinterpret_cast<unsigned short*>(inBytes + offset + offsetDelta); offsetDelta += sizeof(unsigned short);
		unsigned short y = *reinterpret_cast<unsigned short*>(inBytes + offset + offsetDelta); offsetDelta += sizeof(unsigned short);
		MFARGB c = *reinterpret_cast<MFARGB*>(inBytes + offset + offsetDelta); offsetDelta += sizeof(MFARGB);
		tranItem.AddAtom(x, y, c);
	}

	transitionKey = &tranKey;
	transitionItem = &tranItem;
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

void GameOfLifeTransition::TransitionToFile(std::ofstream* toFile)
{
	uint32 countAtoms = static_cast<uint32>(m_Coordinates.size());
	toFile->write((char*)&countAtoms, sizeof(uint32));
	TransitionAtom* atoms = m_Coordinates.data();
	toFile->write((char*)atoms, sizeof(GameOfLifeTransition) * m_Coordinates.size());
}
