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

template <typename Out>
void split(const std::string& s, char delim, Out result)
{
	std::istringstream iss(s);
	std::string item;
	while (std::getline(iss, item, delim))
	{
		*result++ = item;
	}
}

std::vector<std::string> split(const std::string& s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));

	return elems;
}