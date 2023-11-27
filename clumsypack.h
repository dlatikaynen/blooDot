#pragma once

namespace blooDot::ClumsyPack
{
	constexpr char const Noice = 69;

	enum BitwiseTileIndex
	{
		Bung = 0,
		NorthEdge = 1,
		NorthEastCorner = 2,
		EastEdge = 4,
		SouthEastCorner = 8,
		SouthEdge = 16,
		SouthWestCorner = 32,
		WestEdge = 64,
		NorthWestCorner = 128
	};

	inline constexpr BitwiseTileIndex operator|(BitwiseTileIndex a, BitwiseTileIndex b)
	{
		return static_cast<BitwiseTileIndex>(static_cast<int>(a) | static_cast<int>(b));
	}

	constexpr unsigned char const BlobTileRotMat[] = {
		0,0,0,0,
		1,4,16,64,
		5,20,80,65,
		7,28,112,193,
		17,68,17,68,
		21,84,81,Noice,
		23,92,113,197,
		29,116,209,71,
		31,124,241,199,
		85,85,85,85,
		87,93,117,213,
		95,125,245,215,
		119,221,119,221,
		127,253,247,223,
		255,255,255,255
	};

	constexpr unsigned char const SevenBySevenIndex[] = {
		0,4,92,124,116,80,0,
		16,20,87,223,241,21,64,
		29,117,85,71,221,125,112,
		31,253,113,28,127,247,209,
		23,199,213,95,255,245,81,
		5,84,93,119,215,193,17,
		0,1,7,197,69,68,65
	};
}