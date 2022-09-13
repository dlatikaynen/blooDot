#pragma once
#include "pch.h"
#include "savegame.h"
#include <regex>
#include "constants.h"

using namespace std::chrono;

namespace blooDot::Savegame
{
	void Create(int savegameIndex)
	{
		SavegameHeader header;
		_SetLocalTimestampStruct(&header.Created);

		const auto& fileName = _GetFilename(savegameIndex);
		const auto settingsFile = SDL_RWFromFile(fileName.c_str(), "wb");
		if (!settingsFile)
		{
			const auto openError = SDL_GetError();
			ReportError("Could not open savegame file for writing", openError);
			return;
		}

		const auto numWritten = settingsFile->write(
			settingsFile,
			(void*)(&header),
			sizeof(SavegameHeaderStruct),
			1
		);

		if (numWritten != 1)
		{
			const auto writeError = SDL_GetError();
			ReportError("Failed to write savegame header", writeError);
		}

		settingsFile->close(settingsFile);
	}

	void Append(int savegameIndex, bool isAutosave)
	{
		std::cout << savegameIndex << isAutosave;
	}

	void Load(int savegameIndex, int savepointIndex)
	{
		std::cout << savegameIndex << ", " << savepointIndex;
	}

	void Delete(int savegameIndex)
	{
		std::cout << savegameIndex;
	}

	std::string _GetFilename(int savegameIndex)
	{
		std::stringstream index;
		index << savegameIndex;
		return std::regex_replace(SavegameFileName, std::regex("\\$i"), index.str());
	}

	void _SetLocalTimestampStruct(LocalTimestamp* timestamp)
	{
		auto& result = *timestamp;
		auto tp = zoned_time{ current_zone(), system_clock::now() }.get_local_time();
		auto dp = floor<days>(tp);
		year_month_day ymd{ dp };
		hh_mm_ss time{ floor<milliseconds>(tp - dp) };

		const auto& year = ymd.year().operator int();
		result.Year = static_cast<unsigned short>(year);
		const auto& month = ymd.month().operator unsigned int();
		result.Month = static_cast<unsigned char>(month);
		const auto& day = ymd.day().operator unsigned int();
		result.Day = static_cast<unsigned char>(day);

		result.Hour = static_cast<unsigned char>(time.hours().count());
		result.Minute = static_cast<unsigned char>(time.minutes().count());
		result.Second = static_cast<unsigned char>(time.seconds().count());
	}
}