#include "pch.h"
#include "datetime.h"
#include "xlations.h"

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
}