#pragma once

#include "..\dings\dings.h"

class ClumsyPacking
{

public:
	typedef char NeighborConfiguration;

	NeighborConfiguration ConfigurationFromNeighbors(bool nw, bool n, bool ne, bool e, bool se, bool s, bool sw, bool w);
	NeighborConfiguration ConfigurationFromNeighbors(Facings theNeighbors);
	Facings FacingFromConfiguration(NeighborConfiguration neighborConfig);

private:
	static const Facings CLUMSY_PACKING[256];

};