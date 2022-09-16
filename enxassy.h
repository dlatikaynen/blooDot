#pragma once
#ifndef NDEBUG
#include <SDL.h>
#include "huff-comp.h"
#include <fstream>
#include <iostream>
#include <filesystem>

constexpr int const CookReturnCodeSuccess = 0;

int Cook();

#endif