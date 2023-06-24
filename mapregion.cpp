#include "pch.h"
#include "mapregion.h"
#include "constants.h"
#include <iostream>
#include <regex>

using namespace blooDot::Datetime;

namespace blooDot::Map
{
	bool WriteStaticRegion(std::unique_ptr<StaticMapRegionDescriptor>& descriptor)
	{
		const auto& fileName = _GetFilename(descriptor->region.RegionId);
		const auto staticRegionFile = SDL_RWFromFile(fileName.c_str(), "wb");
		if (!staticRegionFile)
		{
			const auto openError = SDL_GetError();
			ReportError("Could not open static region map file for writing", openError);
			return false;
		}
		
		StaticMapRegionFileHeader header;
		header.RegionId = static_cast<unsigned short>(descriptor->region.RegionId);
		SetLocalTimestampStruct(&header.Created);
		const auto numWritten = staticRegionFile->write(
			staticRegionFile,
			(void*)(&header),
			sizeof(StaticMapRegionFileHeaderStruct),
			1
		);

		if (numWritten != 1)
		{
			const auto writeError = SDL_GetError();
			ReportError("Failed to write static region map header", writeError);
			return false;
		}

		/* name */
		if (!_WriteString(staticRegionFile, descriptor->region.RegionName))
		{
			return false;
		}

		/* bounding polygon */
		const short numPolypoints = static_cast<short>(descriptor->region.polygon.size());
		const auto numPolypointsWritten = staticRegionFile->write(
			staticRegionFile,
			(void*)&numPolypoints,
			sizeof(short),
			1
		);

		if (numPolypointsWritten != 1)
		{
			const auto writeError = SDL_GetError();
			ReportError("Failed to write static region map polypoint count", writeError);
			return false;
		}

		for (auto& polypoint : descriptor->region.polygon)
		{
			const auto polypointWritten = staticRegionFile->write(
				staticRegionFile,
				(void*)&polypoint,
				sizeof(PointInWorldStruct),
				1
			);

			if (polypointWritten != 1)
			{
				const auto writeError = SDL_GetError();
				ReportError("Failed to write static region map polypoint", writeError);
				return false;
			}
		}

		/* placements */
		const int32 numPlacements = static_cast<int32>(descriptor->dingPlacement.size());
		const auto numPlacementsWritten = staticRegionFile->write(
			staticRegionFile,
			(void*)&numPlacements,
			sizeof(int32),
			1
		);

		if (numPlacementsWritten != 1)
		{
			const auto writeError = SDL_GetError();
			ReportError("Failed to write static region map placement count", writeError);
			return false;
		}

		for (auto& ding : descriptor->dingPlacement)
		{
			const auto placementWritten = staticRegionFile->write(
				staticRegionFile,
				(void*)&ding,
				sizeof(StaticDingPlacementStruct),
				1
			);

			if (placementWritten != 1)
			{
				const auto writeError = SDL_GetError();
				ReportError("Failed to write static region map placement", writeError);
				return false;
			}
		}

		staticRegionFile->close(staticRegionFile);

		return true;
	}

	bool LoadStaticRegion(int regionId, std::unique_ptr<StaticMapRegionDescriptor>& descriptor)
	{
		const auto& fileName = _GetFilename(regionId);
		const auto staticRegionFile = SDL_RWFromFile(fileName.c_str(), "rb");
		if (!staticRegionFile)
		{
			const auto openError = SDL_GetError();
			ReportError("Could not open static region map file for reading", openError);
			return false;
		}

		StaticMapRegionFileHeader header;
		staticRegionFile->read(staticRegionFile, &header, sizeof(StaticMapRegionFileHeaderStruct), 1);
		const StaticMapRegionFileHeader validator;
		const auto isValid = _memicmp(
			(const void*)(&validator.Preamble0),
			(const void*)(&header.Preamble0),
			sizeof(char) * 8
		);

		if (isValid != 0)
		{
			staticRegionFile->close(staticRegionFile);
			ReportError("Could not load static map region file", "Header corrupted");
			descriptor->region.RegionId = 0;
			return false;
		}

		if (header.RegionId != regionId)
		{
			staticRegionFile->close(staticRegionFile);
			std::stringstream mismatchComposer;
			mismatchComposer
				<< "Region Id requested ("
				<< regionId
				<< ") does not match the one found in the store ("
				<< header.RegionId
				<< ")";

			std::string mismatchError = mismatchComposer.str();
			ReportError("Could not load static region map", mismatchError.c_str());
			descriptor->region.RegionId = 0;
			return false;
		}

		/* name */
		descriptor->region.RegionId = header.RegionId;
		_ReadString(staticRegionFile, &descriptor->region.RegionName);

		/* bounding polygon */
		short polypointCount;
		const auto polypointCountRead = staticRegionFile->read(staticRegionFile, (void*)&polypointCount, sizeof(short), 1);
		if (polypointCountRead != 1)
		{
			ReportError("Could not read static region map", "Failed to read bounding polygon vertex count");
			return false;
		}

		for (auto i = 0; i < polypointCount; ++i)
		{
			PointInWorld polypoint;
			const auto polypointRead = staticRegionFile->read(
				staticRegionFile,
				(void*)&polypoint,
				sizeof(PointInWorldStruct),
				1
			);

			if (polypointRead != 1)
			{
				ReportError("Could not read static region map", "Failed to read bounding polygon vertex");
				return false;
			}

			descriptor->region.polygon.push_back(polypoint);
		}

		/* placements */
		int32 placementCount;
		const auto placementCountRead = staticRegionFile->read(staticRegionFile, (void*)&placementCount, sizeof(int32), 1);
		if(placementCountRead != 1)
		{
			ReportError("Could not read static region map", "Failed to read placement count");
			return false;
		}

		for (auto i = 0; i < placementCount; ++i)
		{
			DingPlacement placement;
			const auto placementRead = staticRegionFile->read(
				staticRegionFile,
				(void*)&placement,
				sizeof(StaticDingPlacementStruct),
				1
			);

			if (placementRead != 1)
			{
				ReportError("Could not read static region map", "Failed to read placement");
				return false;
			}

			descriptor->dingPlacement.push_back(placement);
		}

		return true;
	}

	bool _ReadString(SDL_RWops* file, std::string* sink)
	{
		int32 length;
		auto lenRead = file->read(file, &length, sizeof(int32), 1);
		if (lenRead != 1)
		{
			const auto readError = SDL_GetError();
			ReportError("Failed to read string length", readError);
			return false;
		}

		void* raw = SDL_malloc(length);
		if (raw == nullptr)
		{
			const auto readError = SDL_GetError();
			ReportError("Failed to allocate memory for text to be read from file", readError);
			return false;
		}

		auto contentRead = file->read(file, raw, length, 1);
		if (contentRead != 1)
		{
			const auto contentError = SDL_GetError();
			ReportError("Failed to read string content", contentError);
			return false;
		}


		sink->assign((char*)raw, length);
		if (sink->length() != length)
		{
			ReportError("Failed to read text from file", "Number of allocated bytes does not match number of bytes copied");
			return false;
		}

		SDL_free(raw);

		return true;
	}

	bool _WriteString(SDL_RWops* file, std::string text)
	{
		const auto length = static_cast<int32>(text.length());
		const auto lenWritten = file->write(
			file,
			(void*)(&length),
			sizeof(int32),
			1
		);

		if (lenWritten != 1)
		{
			const auto writeError = SDL_GetError();
			ReportError("Failed to write string length", writeError);
			return false;
		}

		const auto content = text.c_str();
		const auto numWritten = file->write(
			file,
			(void*)content,
			sizeof(char) * length,
			1
		);

		if (numWritten != 1)
		{
			const auto writeError = SDL_GetError();
			ReportError("Failed to write string", writeError);
			return false;
		}

		return true;
	}

	std::string _GetFilename(int regionId)
	{
		std::stringstream index;
		index << regionId;
		return std::regex_replace(StaticRegionFileName, std::regex("\\$i"), index.str());
	}

	void _Define(std::unique_ptr<StaticMapRegionDescriptor>& region, int x, int y, Ding ding, short pixX, short pixY, short angle, DingProps props)
	{
		auto placement = DingPlacement();
		placement.ding = ding;
		placement.props = props;
		placement.fromOriginX = x;
		placement.fromOriginY = y;
		placement.pixOffsetX = pixX;
		placement.pixOffsetY = pixY;
		placement.rotationAngle = angle;
		region->dingPlacement.push_back(placement);
	}
}