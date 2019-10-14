#include <stdlib.h>
#pragma once

enum Layers
{
	Floor = 1,
	Walls = 2,
	Rooof = 4
};

enum Facings
{
	North = 1,
	East = 2,
	South = 4,
	West = 8
};

class Dings
{
public:
	Platform::String^	Name();
	Layers				Layer();

protected:

private:
	Platform::String^	m_Name;
	Layers				m_Layer;
	Facings				m_Facing;
	Facings				m_Coalescing;
};

