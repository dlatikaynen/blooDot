#pragma once
#ifndef NDEBUG
#include <SDL.h>
#include "huff-comp.h"
#include <fstream>
#include <iostream>
#include <filesystem>

constexpr int const CookReturnCodeSuccess = 0;
constexpr int const CookReturnCodeRecipeFail = 0xe1;
constexpr int const CookReturnCodeCopyFail = 0xe2;

int Cook();

#endif