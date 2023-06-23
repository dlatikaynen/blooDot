#pragma once
#include <sstream>

typedef struct LocalTimestampStruct
{
	unsigned char Day = 0;
	unsigned char Month = 0;
	unsigned short Year = 0;
	unsigned char Hour = 0; // local time
	unsigned char Minute = 0;
	unsigned char Second = 0;
} LocalTimestamp;

namespace blooDot::Datetime
{
	void Format(const LocalTimestamp* dateTime, std::stringstream* sink);
	void SetLocalTimestampStruct(LocalTimestamp* timestamp);
}