#pragma once
#ifndef NDEBUG

#include <SDL.h>
#include <fstream>
#include <iostream>
#include <filesystem>

constexpr int const XlateReturnCodeSuccess = 0;

typedef struct XlatableIdentifierStruct
{
	std::string Identifier;
	bool HasEn;
	bool HasDe;
	bool HasFi;
	bool HasUa;
	std::string en;
	std::string de;
	std::string fi;
	std::string ua;
} XlatableIdentifier;

int Xlate();

#endif