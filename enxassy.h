#pragma once
#ifndef NDEBUG
#include <SDL2/SDL.h>
#include "huff-comp.h"
#include <fstream>
#include <iostream>
#include <filesystem>

constexpr int const CookReturnCodeSuccess = 0;
constexpr int const CookReturnCodeRecipeFail = 0xe1;
constexpr int const CookReturnCodeCopyFail = 0xe2;
constexpr int const CookReturnCodeInputFileNotFound = 0xe3;

struct XassyCookInfo {
	int numFiles;
	uintmax_t numBytesBefore;
	uintmax_t numBytesAfter;
};

int Cook(XassyCookInfo* cookStats);
#endif