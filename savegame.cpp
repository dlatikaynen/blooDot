#pragma once
#include "pch.h"
#include "savegame.h"
#include <regex>
#include "constants.h"
#include "settings.h"
#include "resutil.h"

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
		SaveSettings();

		return savegameIndex;
	}

	void Append(int savegameIndex, bool isAutosave, size_t screenshotLength, const void* screenshot)
	{
		if (!(Settings.OccupiedSavegameSlots & (1 << (savegameIndex - 1))))
		{
			ReportError("Could not append to savegame", "Attempt to append to an empty savegame slot");
			return;
		}

		const auto& fileName = _GetFilename(savegameIndex);
		const auto saveFile = SDL_RWFromFile(fileName.c_str(), "ab");
		if (!saveFile)
		{
			const auto openError = SDL_GetError();
			ReportError("Could not open savegame file for append", openError);
			return;
		}

		/* 1. write the descriptor */
		SavepointHeader header;
		_SetLocalTimestampStruct(&header.Written);
		header.DataLength = 0;
		header.ScreenshotLength = static_cast<unsigned int>(screenshotLength);
		header.RegionId = 1;
		header.OriginDx = 0;
		header.OriginDy = 0;
		header.PlayersJoined = 1;
		header.IsAutosave = isAutosave ? 0xff : 0;
		auto numWritten = saveFile->write(
			saveFile,
			(void*)(&header),
			sizeof(SavepointHeaderStruct),
			1
		);

		if (numWritten != 1)
		{
			const auto appendError = SDL_GetError();
			ReportError("Failed to write savepoint header", appendError);
			return;
		}

		/* 2. write the screenshot */
		if (screenshotLength > 0)
		{
			numWritten = saveFile->write(saveFile, screenshot, screenshotLength, 1);
			if (numWritten != 1)
			{
				const auto appendError = SDL_GetError();
				ReportError("Failed to write savepoint screenshot", appendError);
				return;
			}
		}

		/* 3. write the delta frame */

		saveFile->close(saveFile);
	}

	/// <summary>
	/// If this returns 0 as the savegameIndex in the header,
	/// it means that it could not be opened or loaded
	/// renderer: Because we load the screenshot
	/// </summary>
	SavegameChoiceDescriptor LoadInfoShallow(SDL_Renderer* renderer, int savegameIndex)
	{
		SavegameChoiceDescriptor savegameDescriptor;
		savegameDescriptor.Header.SavegameIndex = 0;
		const auto& fileName = _GetFilename(savegameIndex);
		const auto savegameFile = SDL_RWFromFile(fileName.c_str(), "rb");
		if (!savegameFile)
		{
			const auto openError = SDL_GetError();
			ReportError("Could not open savegame", openError);
			return savegameDescriptor;
		}

		savegameFile->read(savegameFile, &savegameDescriptor.Header, sizeof(SavegameHeaderStruct), 1);
		const SavegameHeader validator;
		const auto isValid = _memicmp(
			(const void*)(&validator.Preamble0),
			(const void*)(&savegameDescriptor.Header.Preamble0),
			sizeof(char) * 9
		);

		if (isValid != 0)
		{
			savegameFile->close(savegameFile);
			ReportError("Could not load savegame", "Savegame store corrupted");
			savegameDescriptor.Header.SavegameIndex = 0;
			return savegameDescriptor;
		}

		if (savegameDescriptor.Header.SavegameIndex != savegameIndex)
		{
			savegameFile->close(savegameFile);
			std::stringstream mismatchComposer;
			mismatchComposer 
				<< "Savegame index requested ("
				<< savegameIndex
				<< ") does not match the one found in the store (" 
				<< savegameDescriptor.Header.SavegameIndex 
				<< ")";

			std::string mismatchError = mismatchComposer.str();
			ReportError("Could not load savegame", mismatchError.c_str());
			savegameDescriptor.Header.SavegameIndex = 0;
			return savegameDescriptor;
		}

		size_t octetsLeft = savegameFile->size(savegameFile) - sizeof(SavegameHeaderStruct);
		while (octetsLeft > 0)
		{
			SavepointHeader savepointDescriptor;
			savegameFile->read(savegameFile, &savepointDescriptor, sizeof(SavepointHeaderStruct), 1);
			const SavepointHeader validator2;
			const auto isValid2 = _memicmp(
				(const void*)(&validator2.Preamble0),
				(const void*)(&savepointDescriptor.Preamble0),
				sizeof(char) * 4
			);

			if (isValid2 != 0)
			{
				savegameFile->close(savegameFile);
				ReportError("Could not load savegame", "Savepoint store corrupted");
				savegameDescriptor.Header.SavegameIndex = 0;
				return savegameDescriptor;
			}

			octetsLeft -= sizeof(SavepointHeaderStruct);
			savegameDescriptor.Savepoints.push_back(savepointDescriptor);
			
			/* for the shallow one, we need but the last screenshot */
			SDL_Rect rect;
			if ((octetsLeft - savepointDescriptor.ScreenshotLength - savepointDescriptor.DataLength) == 0)
			{
				savegameDescriptor.MostRecentScreenshot = blooDot::Res::LoadPicture(
					renderer,
					savegameFile,
					&rect
				);
			}
			else if(savepointDescriptor.ScreenshotLength > 0)
			{
				savegameFile->seek(savegameFile, savepointDescriptor.ScreenshotLength, SEEK_CUR);
			}

			/* for the shallow one, we skip stuff */
			if (savepointDescriptor.DataLength > 0)
			{
				savegameFile->seek(savegameFile, savepointDescriptor.DataLength, SEEK_CUR);
			}

			octetsLeft -= savepointDescriptor.ScreenshotLength;
			octetsLeft -= savepointDescriptor.DataLength;
		}

		savegameFile->close(savegameFile);

		return savegameDescriptor;
	}

	void Load(int savegameIndex, int savepointIndex)
	{
		std::cout << savegameIndex << ", " << savepointIndex;
	}

	void Delete(int savegameIndex)
	{
		if (savegameIndex <= 0 || savegameIndex >= 0xf)
		{
			std::cerr << "No such savegame slot: " << savegameIndex;
			return;
		}

		const auto& delResult = _unlink(_GetFilename(savegameIndex).c_str());

		std::cout << savegameIndex << "=>" << delResult;

		Settings.OccupiedSavegameSlots &= ~(1 << (savegameIndex - 1));
		if (Settings.CurrentSavegameIndex == savegameIndex)
		{
			Settings.CurrentSavegameIndex = 0;
		}

		SaveSettings();
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