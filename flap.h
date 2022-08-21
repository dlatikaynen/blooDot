#pragma once
#include <SDL.h>
#include <iostream>
#include "renderstate.h"

constexpr auto const UnsignednessOffset = 0x800;

typedef struct FlapAwareness {
	/// <summary>
	/// where in the X range of flaps am I in the world? origin center being 0
	/// </summary>
	int flapInWorldX;
	
	/// <summary>
	/// where in the Y range of flaps am I in the world? origin middle being 0
	/// </summary>
	int flapInWorldY; 

	/// <summary>
	/// which is the absolute world grid x-coordinate of my first (left) column?
	/// </summary>
	int myGridLeftX;

	/// <summary>
	/// which is the absolute worls grid y-coordinate of my first (top) row?
	/// </summary>
	int myGridToopY;
	
	int numGridUnitsWide;
	int numGridUnitsHigh;

	/// <summary>
	/// where to start drawing on the flap, left
	/// </summary>
	int localDrawingOffsetX;

	/// <summary>
	/// where to start drawing on the flap, top
	/// </summary>
	int localDrawingOffsetY;
} FlapAwareness;

void PopulateFlap(int flapIndex, int flapInWorldX, int flapInWorldY);
int FlapAwarenessLeft();
int FlapAwarenessRite();
int FlapAwarenessUuup();
int FlapAwarenessDown();

FlapAwareness* _FigureOutAwarenessFor(int flapIndex, int flapInWorldX, int flapInWorldY);
