#pragma once
#include "pch.h"
#include "savegame.h"
#include <regex>
#include "constants.h"
#include "settings.h"

using namespace std::chrono;

extern SettingsStruct Settings;

namespace blooDot::Savegame
{
	int Create()
	{
		const auto savegameIndex = _FindFreeSavegameSlot();
		if (savegameIndex == 0)
		{
			return savegameIndex;
		}

		SavegameHeader header;
		header.SavegameIndex = static_cast<unsigned short>(savegameIndex);
		_SetLocalTimestampStruct(&header.Created);

		const auto& fileName = _GetFilename(savegameIndex);
		const auto saveFile = SDL_RWFromFile(fileName.c_str(), "wb");
		if (!saveFile)
		{
			const auto openError = SDL_GetError();
			ReportError("Could not open savegame file for writing", openError);
			return 0;
		}

		const auto numWritten = saveFile->write(
			saveFile,
			(void*)(&header),
			sizeof(SavegameHeaderStruct),
			1
		);

		if (numWritten != 1)
		{
			const auto writeError = SDL_GetError();
			ReportError("Failed to write savegame header", writeError);
			return 0;
		}

		saveFile->close(saveFile);
		Settings.CurrentSavegameIndex = static_cast<unsigned short>(savegameIndex);
		Settings.OccupiedSavegameSlots |= (1 << (savegameIndex - 1));

		return savegameIndex;
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

	int _FindFreeSavegameSlot()
	{
		const auto& slots = Settings.OccupiedSavegameSlots;
		for (auto i = 0; i < 16; ++i)
		{
			if (!(slots & (1 << i)))
			{
				return i + 1;
			}
		}

		return 0;
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