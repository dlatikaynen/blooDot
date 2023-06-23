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

		if (!_WriteString(staticRegionFile, descriptor->region.RegionName))
		{
			return false;
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
			ReportError("Could not load savegame", mismatchError.c_str());
			descriptor->region.RegionId = 0;
			return false;
		}

		descriptor->region.RegionId = header.RegionId;
		_ReadString(staticRegionFile, &descriptor->region.RegionName);

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

	void _Define(std::unique_ptr<StaticMapRegionDescriptor>& region, int x, int y, Ding ding, int pixX, int pixY, int angle, DingProps props)
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