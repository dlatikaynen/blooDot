#pragma once

/* first of all:
 * get rid of that library-dictates-platform-linking bullshit */
#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <iostream>

extern void PrepareIndex();
extern bool OpenCooked();
extern void TeardownDialogControls();
extern void CloseCooked();
extern void CloseFonts();

void _Launch();