#include "pch.h"
#include "harvard.h"

int SetBitCount(unsigned short val)
{
	auto count = 0;
	for (auto i = 0; i < 0xf; ++i)
	{
		if (val & (1 << i))
		{
			++count;
		}
	}

	return count;
}