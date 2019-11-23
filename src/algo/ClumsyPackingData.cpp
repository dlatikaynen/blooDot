#include "ClumsyPacking.h"

// The wonder of the clumsy packing of wang tiles in a 2-topology
// See clumsypack.xlsx for illumination. this set has its own primes,
// its own pi, and its own fine structure constant, probably
const Facings ClumsyPacking::CLUMSY_PACKING[256]{
	Facings::Shy,
	Facings::Shy,
	Facings::South,
	Facings::South
};
