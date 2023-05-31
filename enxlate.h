#pragma once
#ifndef NDEBUG

#include <SDL.h>
#include <fstream>
#include <iostream>
#include <filesystem>

constexpr int const XlateReturnCodeSuccess = 0;

struct XassyXlatInfo {
	int numFiles;
	int numLiterals;
};

typedef struct XlatableIdentifierStruct
{
	std::string Identifier;
	bool HasAm;
	bool HasDe;
	bool HasFi;
	bool HasUa;
	std::string am;
	std::string de;
	std::string fi;
	std::string ua;
} XlatableIdentifier;

int Xlate(XassyXlatInfo*);

#endif