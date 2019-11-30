#pragma once

#include "..\dings\dings.h"

class ClumsyPacking
{

public:
	typedef unsigned char NeighborConfiguration;

	static ClumsyPacking::NeighborConfiguration ClumsyPacking::ConfigurationFromNeighbors(bool nw, bool n, bool ne, bool e, bool se, bool s, bool sw, bool w)
	{
		return nw + (n << 1) + (ne << 2) + (e << 3) + (se << 4) + (s << 5) + (sw << 6) + (w << 7);
	}

	static ClumsyPacking::NeighborConfiguration ClumsyPacking::ConfigurationFromNeighbors(Facings theNeighbors)
	{
		return theNeighbors & 0x000000ff;
	}

	static Facings ClumsyPacking::FacingFromConfiguration(ClumsyPacking::NeighborConfiguration neighborConfig)
	{
		return ClumsyPacking::CLUMSY_PACKING[static_cast<unsigned>(neighborConfig)];
	}

	// The wonder of the clumsy packing of wang tiles in a 2-topology
	// See clumsypack.xlsx for illumination. this set has its own primes,
	// its own pi, and its own fine structure constant, probably
	static constexpr Facings ClumsyPacking::CLUMSY_PACKING[(unsigned)256]
	{
		Facings::Shy, //0
		Facings::Shy, //1
		Facings::South, //2
		Facings::South, //3
		Facings::Shy, //4
		Facings::Shy, //5
		Facings::South, //6
		Facings::South, //7
		Facings::West, //8
		Facings::West, //9
		Facings::EdgeSW, //10
		Facings::EdgeSW, //11
 		Facings::West, //12
		Facings::West, //13
		Facings::EdgeSW, //14
		Facings::EdgeSW, //15
		Facings::Shy, //16
		Facings::Shy, //17
		Facings::South, //18
		Facings::South, //19
		Facings::Shy, //20
		Facings::Shy, //21
		Facings::South, //22
		Facings::South, //23
		Facings::West, //24
		Facings::West, //25
		Facings::EdgeSW, //26
		Facings::EdgeSW, //27
		Facings::West, //28
		Facings::West, //29
		Facings::EdgeSW, //30
		Facings::EdgeSW, //31
		Facings::North, //32
		Facings::North, //33
		Facings::NS, //34
		Facings::NS, //35
		Facings::North, //36
		Facings::North, //37
		Facings::NS, //38
		Facings::NS, //39
		Facings::EdgeCornerNW, //40
		Facings::EdgeCornerNW, //41
		Facings::TE, //42
		Facings::TE, //43
		Facings::EdgeNW, //44
		Facings::EdgeNW, //45
		Facings::EdgeCornerWD, //46
		Facings::EdgeCornerWD, //47
		Facings::North, //48
		Facings::North, //49
		Facings::NS, //50
		Facings::NS, //51
		Facings::North, //52
		Facings::North, //53
		Facings::NS, //54
		Facings::NS, //55
		Facings::EdgeNW, //56
		Facings::EdgeNW, //57
		Facings::EdgeCornerWU, //58
		Facings::EdgeCornerWU, //59
		Facings::EdgeNW, //60
		Facings::EdgeNW, //61
		Facings::EdgeW, //62
		Facings::EdgeW, //63
		Facings::Shy, //64
		Facings::Shy, //65
		Facings::North, //66
		Facings::North, //67
		Facings::Shy, //68
		Facings::Shy, //69
		Facings::North, //70
		Facings::North, //71
		Facings::West, //72
		Facings::West, //73
		Facings::EdgeSW, //74
		Facings::EdgeSW, //75
		Facings::West, //76
		Facings::West, //77
		Facings::EdgeSW, //78
		Facings::EdgeSW, //79
		Facings::Shy, //80
		Facings::Shy, //81
		Facings::South, //82
		Facings::South, //83
		Facings::Shy, //84
		Facings::Shy, //85
		Facings::South, //86
		Facings::South, //87
		Facings::West, //88
		Facings::West, //89
		Facings::EdgeCornerSW, //90
		Facings::EdgeCornerSW, //91
		Facings::West, //92
		Facings::West, //93
		Facings::EdgeSW, //94
		Facings::EdgeSW, //95
		Facings::North, //96
		Facings::North, //97
		Facings::NS, //98
		Facings::NS, //99
		Facings::North, //100
		Facings::North, //101
		Facings::NS, //102
		Facings::NS, //103
		Facings::EdgeCornerNW, //104
		Facings::EdgeCornerNW, //105
		Facings::TE, //106
		Facings::TE, //107
		Facings::EdgeCornerNW, //108
		Facings::EdgeCornerNW, //109
		Facings::EdgeCornerWD, //110
		Facings::EdgeCornerWD, //111
		Facings::North, //112
		Facings::North, //113
		Facings::NS, //114
		Facings::NS, //115
		Facings::North, //116
		Facings::North, //117
		Facings::NS, //118
		Facings::NS, //119
		Facings::EdgeNW, //120
		Facings::EdgeNW, //121
		Facings::EdgeCornerWU, //122
		Facings::EdgeCornerWU, //123
		Facings::EdgeNW, //124
		Facings::EdgeNW, //125
		Facings::EdgeW, //126
		Facings::EdgeW, //127
		Facings::East, //128
		Facings::East, //129
		Facings::EdgeSE, //130
		Facings::EdgeSE, //131
		Facings::East, //132
		Facings::East, //133
		Facings::EdgeSE, //134
		Facings::EdgeSE, //135
		Facings::WE, //136
		Facings::WE, //137
		Facings::TN, //138
		Facings::EdgeCornerSR, //139
		Facings::WE, //140
		Facings::WE, //141
		Facings::EdgeCornerSL, //142
		Facings::EdgeS, //143
		Facings::East, //144
		Facings::East, //145
		Facings::EdgeSE, //146
		Facings::EdgeSE, //147
		Facings::East, //148
		Facings::East, //149
		Facings::EdgeSE, //150
		Facings::EdgeSE, //151
		Facings::WE, //152
		Facings::WE, //153
		Facings::TN, //154
		Facings::EdgeCornerSR, //155
		Facings::WE, //156
		Facings::WE, //157
		Facings::EdgeCornerSL, //158
		Facings::EdgeS, //159
		Facings::EdgeNE, //160
		Facings::EdgeNE, //161
		Facings::TW, //162
		Facings::EdgeCornerED, //163
		Facings::EdgeNE, //164
		Facings::EdgeNE, //165
		Facings::TW, //166
		Facings::EdgeCornerED, //167
		Facings::TS, //168
		Facings::TS, //169
		Facings::Center, //170
		Facings::CornerSWNE, //171
		Facings::TS, //172
		Facings::TS, //173
		Facings::Corner3SW, //174
		Facings::Corner2S, //175
		Facings::EdgeCornerNE, //176
		Facings::EdgeCornerNE, //177
		Facings::TW, //178
		Facings::EdgeCornerED, //179
		Facings::EdgeNE, //180
		Facings::EdgeNE, //181
		Facings::TW, //182
		Facings::EdgeCornerED, //183
		Facings::EdgeCornerNL, //184
		Facings::EdgeCornerNL, //185
		Facings::Corner3NW, //186
		Facings::CornerSWNE, //187
		Facings::EdgeCornerNL, //188
		Facings::EdgeCornerNL, //189
		Facings::Corner2W, //190
		Facings::Corner1SW, //191
		Facings::East, //192
		Facings::East, //193
		Facings::EdgeSE, //194
		Facings::EdgeSE, //195
		Facings::East, //196
		Facings::East, //197
		Facings::EdgeSE, //198
		Facings::EdgeSE, //199
		Facings::WE, //200
		Facings::WE, //201
		Facings::TN, //202
		Facings::EdgeCornerSR, //203
		Facings::WE, //204
		Facings::WE, //205
		Facings::EdgeCornerSL, //206
		Facings::EdgeS, //207
		Facings::East, //208
		Facings::East, //209
		Facings::EdgeCornerSE, //210
		Facings::EdgeSE, //211
		Facings::East, //212
		Facings::East, //213
		Facings::EdgeSE, //214
		Facings::EdgeSE, //215
		Facings::WE, //216
		Facings::WE, //217
		Facings::TN, //218
		Facings::EdgeCornerSR, //219
		Facings::WE, //220
		Facings::WE, //221
		Facings::EdgeCornerSL, //222
		Facings::EdgeS, //223
		Facings::EdgeNE, //224
		Facings::EdgeNE, //225
		Facings::EdgeCornerEU, //226
		Facings::EdgeE, //227
		Facings::EdgeNE, //228
		Facings::EdgeNE, //229
		Facings::EdgeCornerEU, //230
		Facings::EdgeE, //231
		Facings::EdgeCornerNR, //232
		Facings::EdgeCornerNR, //233
		Facings::Corner3NE, //234
		Facings::Corner2E, //235
		Facings::EdgeCornerNR, //236
		Facings::EdgeCornerNR, //237
		Facings::CornerNWSE, //238
		Facings::Corner1SE, //239
		Facings::EdgeNE, //240
		Facings::EdgeNE, //241
		Facings::EdgeCornerEU, //242
		Facings::EdgeE, //243
		Facings::EdgeNE, //244
		Facings::EdgeNE, //245
		Facings::EdgeCornerEU, //246
		Facings::EdgeE, //247
		Facings::EdgeN, //248
		Facings::EdgeN, //249
		Facings::Corner2N, //250
		Facings::Corner1NE, //251
		Facings::EdgeN, //252
		Facings::EdgeN, //253
		Facings::Corner1NW, //254
		Facings::Immersed //255
	};
};