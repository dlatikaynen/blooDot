#pragma once
#include <SDL.h>
#include <iostream>
#include "playerstate.h"
#include "datetime.h"

/* storage format:
 * [1] SavegameHeaderStruct 
 * [2] As many SavepointHeaderStructs as needed
 *     SavepointHeader.RegionNameLength chars of region name;
 *     SavepointHeader.ScreenshotLength chars of screenshot picture;
 *     SavepointHeader.DataLength chars of incremental gamestate.
 */

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
	char Preamble4 = '\0';
	LocalTimestamp Written = { 0 };
	int RegionId = 0;
	unsigned int ScreenshotLength = 0;
	unsigned long long DataLength = 0;
	int OriginDx = 0; // from gameview
	int OriginDy = 0; // from gameview
	char PlayersJoined = 0; // bitfield, bit0 = player 1
	PlayerState Player1State = { 0 };
	PlayerState Player2State = { 0 };
	PlayerState Player3State = { 0 };
	PlayerState Player4State = { 0 };
	char IsAutosave = 0;

} SavepointHeader;

typedef struct SavegameChoiceDescriporStruct
{
	SavegameHeader Header;
	std::vector<SavepointHeader> Savepoints;
	std::vector<SDL_Texture*> Screenshots;
	SDL_Texture* MostRecentScreenshot = NULL;
} SavegameChoiceDescriptor;

namespace blooDot::Savegame
{
	/// <summary>
	/// Returns the 1-based savegame index
	/// Returns 0 if no free slot was available
	/// (in which case the user should be asked to overwrite an existing one
	/// or load an existing one)
	/// </summary>
	int Create();
	void Append(int savegameIndex, bool isAutosave, size_t screenshotLength, const void* screenshot);
	SavegameChoiceDescriptor LoadInfoShallow(SDL_Renderer* renderer, int savegameIndex);
	void Load(int savegameIndex, int savepointIndex);

	/// <summary>
	/// Frees the slot and physically removes the BOFA file
	/// </summary>
	void Delete(int savegameIndex);

	std::string _GetFilename(int savegameIndex);
	int _FindFreeSavegameSlot();
	void _SetLocalTimestampStruct(LocalTimestamp* timestamp);
}
