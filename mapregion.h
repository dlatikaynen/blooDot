#pragma once
#include <vector>
#include "gamestate.h"
#include <memory>
#include "datetime.h"

namespace blooDot::Map
{
	/* describes a single map region as it appears on the map.
	 * does not include the absolute position inside the world.
	 * represents that region in the state in which it is shipped
	 * with the game, and as it is initialized when a new game
	 * is started.
	 * represents the storage format of that data in what we
	 * might call "level files", and is immediately serializable */

	typedef struct StaticDingPlacementStruct
	{
		Ding ding = Ding::None;
		DingProps props = DingProps::Default;

		/// <summary>
		/// grid position in the region (0 is center)
		/// </summary>
		int32 fromOriginX = 0;
		int32 fromOriginY = 0;

		/// <summary>
		/// fine tuning of the placement
		/// </summary>
		short pixOffsetX = 0;
		short pixOffsetY = 0;
		short rotationAngle = 0;
	} DingPlacement;

	typedef struct StaticMapRegionDescriptorStruct {
		WorldRegion region;
		std::vector<DingPlacement> dingPlacement;
		SDL_Texture* designtimeRendition;
	} StaticMapRegionDescriptor;

	typedef struct StaticMapRegionFileHeaderStruct
	{
		char Preamble0 = '\3';
		char Preamble1 = 'L';
		char Preamble2 = 'L';
		char Preamble3 = '\5';
		char Preamble4 = 'J';
		char Preamble5 = 'L';
		char Preamble6 = '\4';
		char Preamble7 = '\0';
		unsigned short RegionId = 0; // must match the file number
		LocalTimestamp Created = { 0 };
	} StaticMapRegionFileHeader;

	typedef struct DingOnMapPositioningStruct
	{
		int x; 
		int y;
		Ding ding;
		short pixX = 0;
		short pixY = 0;
		short angle = 0;
		DingProps props = DingProps::Default;
	} DingOnMapPositioning;

	bool LoadStaticRegion(int regionId, std::unique_ptr<StaticMapRegionDescriptor>& descriptor);
	bool WriteStaticRegion(std::unique_ptr<StaticMapRegionDescriptor>& descriptor);

	bool _WriteString(SDL_RWops* file, std::string text);
	bool _ReadString(SDL_RWops* file, std::string* sink);
	std::string _GetFilename(int regionId);
	void _Define(std::unique_ptr<StaticMapRegionDescriptor>& region, DingOnMapPositioning positioning);
	void _FloorTileUnlessCovered(std::unique_ptr<StaticMapRegionDescriptor>& region, DingOnMapPositioning positioning);
	bool _IsFloorTileCovered(std::unique_ptr<StaticMapRegionDescriptor>& region, DingOnMapPositioning positioning);
}