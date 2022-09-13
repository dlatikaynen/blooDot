#pragma once
#include <SDL.h>
#include <iostream>
#include "playerstate.h"

/* storage format:
 * [1] SavegameHeaderStruct 
 * [2] As many SavepointHeaderStructs as needed
 *     SavepointHeader.RegionNameLength chars of region name;
 *     SavepointHeader.ScreenshotLength chars of screenshot picture;
 *     SavepointHeader.DataLength chars of incremental gamestate.
 */

typedef struct LocalTimestampStruct
{
	unsigned char Day = 0;
	unsigned char Month = 0;
	unsigned short Year = 0;
	unsigned char Hour = 0; // local time
	unsigned char Minute = 0;
	unsigned char Second = 0;
} LocalTimestamp;

namespace blooDot::Savegame
{
	void Create(int savegameIndex);
	void Append(int savegameIndex, bool isAutosave);
	void Load(int savegameIndex, int savepointIndex);
	void Delete(int savegameIndex);

	std::string _GetFilename(int savegameIndex);
	void _SetLocalTimestampStruct(LocalTimestamp* timestamp);
}

typedef struct SavegameHeaderStruct
{
	char Preamble0 = '\3';
	char Preamble1 = 'J';
	char Preamble2 = 'M';
	char Preamble3 = 'L';
	char Preamble4 = '\5';
	char Preamble5 = 'L';
	char Preamble6 = 'S';
	char Preamble7 = 'L';
	char Preamble8 = '\4';
	char Preamble9 = '\0';
	unsigned short SavegameIndex = 0; // must match the file number
	LocalTimestamp Created = { 0 };
	char MultiplayerCode[8] = { 0 };
} SavegameHeader;

typedef struct SavepointHeaderStruct
{
	char Preamble0 = '\2';
	char Preamble1 = 'L';
	char Preamble2 = 'J';
	char Preamble3 = 'L';
	char Preamble9 = '\0';
	LocalTimestamp Written = { 0 };
	unsigned char RegionNameLength = 0;
	unsigned int ScreenshotLength = 0;
	unsigned long long DataLength;
	int OriginDx = 0; // from gameview
	int OriginDy = 0; // from gameview
	char PlayersJoined = 0; // bitfield, bit0 = player 1
	PlayerState Player1State;
	PlayerState Player2State;
	PlayerState Player3State;
	PlayerState Player4State;

} SavepointHeader;