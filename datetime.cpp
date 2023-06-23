#include "pch.h"
#include "datetime.h"
#include "xlations.h"

using namespace std::chrono;

namespace blooDot::Datetime
{
	void Format(const LocalTimestamp* dateTime, std::stringstream* sink)
	{
		auto& formatter = *sink;
		auto lcid = std::string(literalLCID);
		if(lcid == "de" || lcid == "ua")
		{
			formatter
				<< std::setw(2) << std::setfill('0')
				<< (int)dateTime->Day
				<< "."
				<< std::setw(2) << (int)dateTime->Month
				<< "."
				<< (int)dateTime->Year
				<< " "
				<< std::setw(2) << (int)dateTime->Hour
				<< ":"
				<< std::setw(2) << (int)dateTime->Minute
				<< ":"
				<< std::setw(2) << (int)dateTime->Second;

			return;
		}

		if (lcid == "fi")
		{
			formatter
				<< std::setw(2) << std::setfill('0')
				<< (int)dateTime->Day
				<< "."
				<< std::setw(2) << (int)dateTime->Month
				<< "."
				<< (int)dateTime->Year
				<< " "
				<< std::setw(2) << (int)dateTime->Hour
				<< "."
				<< std::setw(2) << (int)dateTime->Minute
				<< "."
				<< std::setw(2) << (int)dateTime->Second;

			return;
		}

		formatter 
			<< std::setw(2) << std::setfill('0') 
			<< (int)dateTime->Day
			<< "/"
			<< std::setw(2) << (int)dateTime->Month
			<< "/"
			<< (int)dateTime->Year
			<< " "
			<< std::setw(2) << (int)dateTime->Hour
			<< ":"
			<< std::setw(2) << (int)dateTime->Minute
			<< ":"
			<< std::setw(2) << (int)dateTime->Second;
	}

	void SetLocalTimestampStruct(LocalTimestamp* timestamp)
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