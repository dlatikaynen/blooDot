#include "pch.h"
#include "harvard.h"

int SetBitCount(unsigned short val)
{
	unsigned int count = 0;
	while (val)
	{
		val &= (val - 1);
		++count;
	}
	
	return count;
}