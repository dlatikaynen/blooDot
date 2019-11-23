#include "ClumsyPacking.h"

ClumsyPacking::NeighborConfiguration ClumsyPacking::ConfigurationFromNeighbors(bool nw, bool n, bool ne, bool e, bool se, bool s, bool sw, bool w)
{
	return nw + n << 1 + ne << 2 + e << 3 + se << 4 + s << 5 + sw << 6 + w << 7;
}

ClumsyPacking::NeighborConfiguration ClumsyPacking::ConfigurationFromNeighbors(Facings theNeighbors)
{
	return theNeighbors & 0x000000ff;
}

Facings ClumsyPacking::FacingFromConfiguration(ClumsyPacking::NeighborConfiguration neighborConfig)
{
	return this->CLUMSY_PACKING[neighborConfig];
}
