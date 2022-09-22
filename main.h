#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <iostream>

extern void PrepareIndex();
extern bool OpenCooked();
extern void TeardownDialogControls();
extern void CloseCooked();
extern void CloseFonts();

void _Launch();